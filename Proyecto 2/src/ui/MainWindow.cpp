// Copyright Alejandro, Ashley y Kevin
#include "MainWindow.hpp"

#include <QCoreApplication>
#include <QGuiApplication>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMessageBox>
#include <QPoint>
#include <QRect>
#include <QScreen>
#include <QStackedWidget>
#include <QStatusBar>
#include <QString>
#include <QTimer>
#include <cstdlib>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "AIController.hpp"
#include "BoardTheme.hpp"
#include "GameBoardWidget.hpp"
#include "GameEngine.hpp"
#include "GameLogger.hpp"
#include "GameOverDialog.hpp"
#include "GameSetup.hpp"
#include "GameView.hpp"
#include "HudTheme.hpp"
#include "HudWidget.hpp"
#include "OnlineSession.hpp"
#include "StartWindow.hpp"
#include "TurnHandoffDialog.hpp"
#include "UiControls.hpp"
#include "UiTexts.hpp"

namespace {

/**
 * @brief Converts a pressed key into a movement direction.
 * @param key Qt key code (arrows or WASD).
 * @param direction Receives the direction when the key is a movement key.
 * @return True if the key is a movement key, false otherwise.
 */
bool directionFromKey(int key, Direction& direction) {
  switch (key) {
    case Qt::Key_Up:
    case Qt::Key_W:
      direction = Direction::UP;
      return true;
    case Qt::Key_Down:
    case Qt::Key_S:
      direction = Direction::DOWN;
      return true;
    case Qt::Key_Left:
    case Qt::Key_A:
      direction = Direction::LEFT;
      return true;
    case Qt::Key_Right:
    case Qt::Key_D:
      direction = Direction::RIGHT;
      return true;
    default:
      return false;
  }
}

}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle(TEXT_WINDOW_TITLE);

  startWindow_ = new StartWindow;
  boardWidget_ = new GameBoardWidget;
  hudWidget_ = new HudWidget;

  // Extra: Initialize A* search visualizer and search button
  searchVisualizer_ = new AStarVisualizer(boardWidget_, this);
  searchButton_ = new QPushButton(TEXT_BUTTON_SHOW_SEARCH, this);
  searchButton_->setFocusPolicy(Qt::NoFocus);
  hudWidget_->layout()->addWidget(searchButton_);
  connect(searchButton_, &QPushButton::clicked, this,
          &MainWindow::toggleSearchView);
  connect(searchVisualizer_, &AStarVisualizer::finished, this,
          [this]() { searchButton_->setText(TEXT_BUTTON_HIDE_SEARCH); });

  // Match page: the board on the left and the HUD panel on the right
  gamePage_ = new QWidget;
  auto* gameLayout = new QHBoxLayout(gamePage_);
  gameLayout->setContentsMargins(0, 0, 0, 0);
  gameLayout->setSpacing(0);
  gameLayout->addWidget(boardWidget_, 1);
  gameLayout->addWidget(hudWidget_);

  pages_ = new QStackedWidget(this);
  pages_->addWidget(startWindow_);
  pages_->addWidget(gamePage_);
  setCentralWidget(pages_);

  connect(startWindow_, &StartWindow::gameStartRequested, this,
          &MainWindow::startMatch);

  // Extra: Network
  connect(startWindow_, &StartWindow::hostRequested, this,
          &MainWindow::startHosting);
  connect(startWindow_, &StartWindow::joinRequested, this,
          &MainWindow::joinMatch);

  online_ = new OnlineSession(this);
  connect(online_, &OnlineSession::playerAssigned, this,
          &MainWindow::onPlayerAssigned);
  connect(online_, &OnlineSession::viewReceived, this,
          &MainWindow::onViewReceived);
  connect(online_, &OnlineSession::matchEnded, this, &MainWindow::onMatchEnded);
  connect(online_, &OnlineSession::errorOccurred, this,
          &MainWindow::onSessionError);
  connect(online_, &OnlineSession::notice, this, &MainWindow::onNotice);
  connect(online_, &OnlineSession::connectionClosed, this,
          &MainWindow::onConnectionClosed);

  resize(startWindow_->sizeHint());
}

MainWindow::~MainWindow() = default;

void MainWindow::startMatch(const GameOptions& options) {
  hudWidget_->setPrivateInfoHidden(false);
  try {
    engine_ = std::make_unique<GameEngine>(GameSetup::createGame(options));
    aiRng_.seed(options.seed);
    aiController_ = std::make_unique<AIController>();  // fresh path per match
  } catch (const std::runtime_error&) {
    QMessageBox::warning(this, TEXT_WINDOW_TITLE, TEXT_ERROR_MAP_GENERATION);
    return;
  }

  // Extra: Hide search button if fog of war is enabled to prevent cheating
  searchButton_->setVisible(!options.fogEnabled);

  refreshView();  // the board needs the state before measure the grid
  if (!validateBoardFits()) {
    QCoreApplication::exit(EXIT_FAILURE);
    return;
  }

  // Start with the preferred size (board + hud) but never bigger than the
  // screen
  const QSize boardHint = boardWidget_->sizeHint();
  resize(QSize(boardHint.width() + HUD_WIDTH, boardHint.height())
             .boundedTo(availableScreenSize()));
  pages_->setCurrentWidget(gamePage_);

  setFocus();  // so the window, not a hidden menu button, receives the keys
  scheduleAiTurn();
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  Direction direction = Direction::DOWN;
  const bool playing =
      (engine_ || onlineActive_) && pages_->currentWidget() == gamePage_;

  if (!playing || !directionFromKey(event->key(), direction)) {
    QMainWindow::keyPressEvent(event);
    return;
  }

  Action action;
  action.type = (event->modifiers() & JUMP_WALL_MODIFIER)
                    ? ActionType::JUMP_WALL
                    : ActionType::MOVE;
  action.direction = direction;
  submitPlayerAction(action);
}

void MainWindow::submitPlayerAction(const Action& action) {
  // Extra: Network. Online: the server decides, so the window only sends the
  // action
  if (onlineActive_) {
    if (onlineShown_ && !onlineEnded_) {
      online_->sendAction(action);
    }
    return;
  }

  if (!engine_ || engine_->isGameOver() || isAiTurn() || handoffPending_) {
    return;  // no actions after the match is over or during the AI turn
  }

  // Extra: animated A* search visualizer
  searchVisualizer_->stopAnimation();
  searchButton_->setText(TEXT_BUTTON_SHOW_SEARCH);

  const int previousPlayer = engine_->currentPlayer();
  if (!engine_->submitAction(action)) {
    return;  // rejected: nothing changed
  }

  // Show the final board and the winner
  if (engine_->isGameOver()) {
    refreshView();
    finishGame();
    return;
  }

  // In a local match, first show the result of the move to the player who
  // made it, and cover the board only after a short pause
  const bool localMatch =
      engine_->state().options.mode == GameMode::LOCAL_TWO_PLAYERS;

  if (localMatch && engine_->currentPlayer() != previousPlayer) {
    drawBoard(previousPlayer);
    handoffPending_ = true;

    QTimer::singleShot(HANDOFF_PREVIEW_DELAY_MS, this,
                       &MainWindow::finishHandoff);
    return;
  }

  refreshView();
  scheduleAiTurn();
}

void MainWindow::finishHandoff() {
  if (engine_ && !engine_->isGameOver()) {
    showTurnHandoff();
  }
  handoffPending_ = false;
}

void MainWindow::drawBoard(int viewer) {
  boardWidget_->setGameState(engine_->state());
  boardWidget_->setViewer(viewer);
  boardWidget_->setVisibleCells(engine_->visibleCells(viewer));
}

void MainWindow::refreshView() {
  if (!engine_) {
    return;
  }

  const GameState& state = engine_->state();
  const int viewer = state.options.mode == GameMode::SINGLE_PLAYER
                         ? PLAYER_1
                         : state.currentPlayer;

  drawBoard(viewer);
  hudWidget_->updateFromState(state);
}

bool MainWindow::isAiTurn() const {
  return engine_ && !engine_->isGameOver() &&
         engine_->state().options.mode == GameMode::SINGLE_PLAYER &&
         engine_->currentPlayer() == PLAYER_2;
}

void MainWindow::scheduleAiTurn() {
  if (!isAiTurn()) {
    return;
  }
  QTimer::singleShot(AI_ACTION_DELAY_MS, this, &MainWindow::runAiTurn);
}

void MainWindow::runAiTurn() {
  // The state may have changed while the timer was waiting
  if (!isAiTurn()) {
    return;
  }

  // Extra: animated A* search visualizer
  searchVisualizer_->stopAnimation();
  searchButton_->setText(TEXT_BUTTON_SHOW_SEARCH);

  const Action action = aiController_->chooseAction(engine_->state(), aiRng_);

  // Hand the search (if the AI planned) to the engine for the game log
  const std::optional<SearchRecord> record =
      aiController_->takeLatestSearchRecord();
  if (record.has_value()) {
    engine_->recordAiSearch(*record);
  }

  if (!engine_->submitAction(action)) {
    return;  // rejected: stop here instead of retrying forever
  }

  refreshView();

  if (engine_->isGameOver()) {
    finishGame();
    return;
  }

  scheduleAiTurn();
}

void MainWindow::showTurnHandoff() {
  const char* name = engine_->currentPlayer() == PLAYER_1 ? TEXT_HUD_PLAYER_1
                                                          : TEXT_HUD_PLAYER_2;

  TurnHandoffDialog dialog(QString::fromLatin1(name), this);

  // Cover the whole game area, and hide the board while the dialog is open
  // so no part of the maze can show through
  QWidget* area = centralWidget();
  dialog.setGeometry(QRect(area->mapToGlobal(QPoint(0, 0)), area->size()));
  gamePage_->setVisible(false);
  dialog.exec();
  refreshView();
  gamePage_->setVisible(true);
}

QSize MainWindow::availableScreenSize() const {
  const QScreen* screen = QGuiApplication::primaryScreen();
  const QSize screenSize =
      screen ? screen->availableGeometry().size() : QSize();

  return QSize(screenSize.width() - BOARD_SCREEN_RESERVED_WIDTH,
               screenSize.height() - BOARD_SCREEN_RESERVED_HEIGHT);
}

bool MainWindow::validateBoardFits() {
  // Area left for the grid once the board margins are removed
  const QSize available = availableScreenSize();
  const QSize gridArea(available.width() - HUD_WIDTH - 2 * BOARD_MARGIN,
                       available.height() - 2 * BOARD_MARGIN);

  // The maze fits if cells can be at least as big as the readable minimum
  if (boardWidget_->calculateCellSize(gridArea) >= BOARD_MIN_CELL_SIZE) {
    return true;
  }

  const QSize grid = boardWidget_->gridSize();  // width = cols, height = row

  QMessageBox::critical(this, TEXT_WINDOW_TITLE,
                        QString::fromLatin1(TEXT_ERROR_BOARD_DOES_NOT_FIT)
                            .arg(grid.height())
                            .arg(grid.width()));

  return false;
}

void MainWindow::finishGame() {
  if (!engine_ || !engine_->isGameOver()) {
    return;
  }

  const GameState& state = engine_->state();

  // Write the game log once, with the same state shown in the dialog
  writeGameLog(state);

  const bool singlePlayer = state.options.mode == GameMode::SINGLE_PLAYER;
  GameOverDialog dialog(singlePlayer, this);
  dialog.showWinner(state.winner);

  if (!dialog.newGameRequested()) {
    close();
    return;
  }

  // New game, back to the menu so the user can choose the options again
  showMenu();
}

void MainWindow::toggleSearchView() {
  if (searchVisualizer_->isRunning() ||
      searchButton_->text() == TEXT_BUTTON_HIDE_SEARCH) {
    searchVisualizer_->stopAnimation();
    searchButton_->setText(TEXT_BUTTON_SHOW_SEARCH);
    return;
  }
  const GameState& state = engine_->state();
  SearchResult result =
      findPathAStar(state.maze, state.players[state.currentPlayer].position,
                    state.treasure, true);
  searchVisualizer_->playTrace(result.trace);
}

void MainWindow::writeGameLog(const GameState& state) {
  const std::string logFile = GameLogger::makeLogFilename();
  if (!GameLogger::writeLog(state, logFile)) {
    QMessageBox::warning(this, TEXT_WINDOW_TITLE,
                         QString::fromLatin1(TEXT_ERROR_GAME_LOG)
                             .arg(QString::fromStdString(logFile)));
  }
}

void MainWindow::showMenu() {
  online_->close();
  onlineActive_ = false;
  startWindow_->setEnabled(true);
  statusBar()->clearMessage();
  hudWidget_->setPrivateInfoHidden(false);

  engine_.reset();
  aiController_.reset();
  pages_->setCurrentWidget(startWindow_);
  resize(startWindow_->sizeHint());
}

void MainWindow::beginOnline() {
  onlineActive_ = true;
  onlineShown_ = false;
  onlineEnded_ = false;
  localPlayerId_ = -1;
  hudWidget_->setPrivateInfoHidden(
      true);                        // the protocol does not send the counts
  startWindow_->setEnabled(false);  // no second click while connecting
  statusBar()->showMessage(TEXT_ONLINE_WAITING);
}

void MainWindow::startHosting(const GameOptions& options, quint16 port) {
  beginOnline();
  if (!online_->host(options, port)) {
    QMessageBox::warning(this, TEXT_WINDOW_TITLE, TEXT_ERROR_ONLINE_START);
    showMenu();
  }
}

void MainWindow::joinMatch(const QString& address, quint16 port) {
  beginOnline();
  if (!online_->join(address.toStdString(), port)) {
    QMessageBox::warning(this, TEXT_WINDOW_TITLE, TEXT_ERROR_ONLINE_START);
    showMenu();
  }
}

void MainWindow::onPlayerAssigned(int playerId) {
  if (!onlineActive_) {
    return;
  }
  localPlayerId_ = playerId;
  const char* name =
      playerId == PLAYER_1 ? TEXT_HUD_PLAYER_1 : TEXT_HUD_PLAYER_2;
  statusBar()->showMessage(
      QString::fromLatin1(TEXT_ONLINE_YOU_ARE).arg(QString::fromLatin1(name)));
}

void MainWindow::onViewReceived(const GameView& view) {
  if (!onlineActive_) {
    return;
  }

  boardWidget_->setGameView(view);
  boardWidget_->setViewer(localPlayerId_);
  hudWidget_->updateFromState(boardWidget_->state());

  if (!onlineShown_) {
    onlineShown_ = true;
    if (!validateBoardFits()) {
      showMenu();
      return;
    }
    const QSize boardHint = boardWidget_->sizeHint();
    resize(QSize(boardHint.width() + HUD_WIDTH, boardHint.height())
               .boundedTo(availableScreenSize()));
    pages_->setCurrentWidget(gamePage_);
    setFocus();
  }
}

void MainWindow::onMatchEnded(int winner) {
  if (!onlineActive_ || onlineEnded_) {
    return;
  }
  onlineEnded_ = true;

  // Only the host has the final state, so only it writes the game log
  const GameState* finalState = online_->authoritativeState();
  if (finalState != nullptr) {
    writeGameLog(*finalState);
  }

  GameOverDialog dialog(false, this);
  dialog.showWinner(winner);
  if (!dialog.newGameRequested()) {
    close();
    return;
  }
  showMenu();
}

void MainWindow::onSessionError(const QString& error) {
  if (!onlineActive_ || onlineEnded_) {
    return;
  }
  QMessageBox::warning(
      this, TEXT_WINDOW_TITLE,
      QString::fromLatin1(TEXT_ERROR_ONLINE_CONNECTION).arg(error));
  showMenu();
}

void MainWindow::onNotice(const QString& message) {
  if (onlineActive_) {
    statusBar()->showMessage(message, ONLINE_MESSAGE_MS);
  }
}

void MainWindow::onConnectionClosed() {
  if (!onlineActive_ || onlineEnded_) {
    return;
  }
  QMessageBox::warning(this, TEXT_WINDOW_TITLE, TEXT_ERROR_ONLINE_CLOSED);
  showMenu();
}
