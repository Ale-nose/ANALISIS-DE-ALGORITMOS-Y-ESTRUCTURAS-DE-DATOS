// Copyright Alejandro, Ashley y Kevin
#include "MainWindow.hpp"
 
#include <QCoreApplication>
#include <QGuiApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <QScreen>
#include <QStackedWidget>
#include <QString>
#include <cstdlib>
#include <stdexcept>
#include <utility>
 
#include "BoardTheme.hpp"
#include "GameBoardWidget.hpp"
#include "GameEngine.hpp"
#include "GameSetup.hpp"
#include "StartWindow.hpp"
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
    switch (key){
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

  pages_ = new QStackedWidget(this);
  pages_->addWidget(startWindow_);
  pages_->addWidget(boardWidget_);
  setCentralWidget(pages_);

  connect(startWindow_, &StartWindow::gameStartRequested, this
    , &MainWindow::startMatch);
  resize(startWindow_->sizeHint());
}

MainWindow::~MainWindow() = default;

void MainWindow::startMatch(const GameOptions& options) {
  try {
    engine_ = std::make_unique<GameEngine>(GameSetup::createGame(options));
  } catch (const std::runtime_error&) {
    QMessageBox::warning(this, TEXT_WINDOW_TITLE, TEXT_ERROR_MAP_GENERATION);
    return;
  }

  refreshView();
  if (!validateBoardFits()) {
    QCoreApplication::exit(EXIT_FAILURE);
    return;
  }

  // Start with the preferred size but never bigger than the screen
  resize(boardWidget_->sizeHint().boundedTo(availableScreenSize()));
  pages_->setCurrentWidget(boardWidget_);
  setFocus();  // so the window, not a hidden menu button, receives the keys
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
  Direction direction = Direction::DOWN;
  const bool playing = engine_ && pages_->currentWidget() == boardWidget_;

  if (!playing || !directionFromKey(event->key(), direction)) {
    QMainWindow::keyPressEvent(event);
    return;
  }

  Action action;
  action.type = (event->modifiers() & JUMP_WALL_MODIFIER) 
    ? ActionType::JUMP_WALL : ActionType::MOVE;
  action.direction = direction;
  submitPlayerAction(action);
}

void MainWindow::submitPlayerAction(const Action& action) {
  if (!engine_) {
    return;
  }
  if (engine_->submitAction(action)) {
    refreshView();
  }
  // TODO: if it is the AI's turn (single player), run it here.
}

void MainWindow::refreshView() {
  if (!engine_) {
    return;
  }

  boardWidget_->setGameState(engine_->state());
  boardWidget_->update();
   // TODO: update the HUD, the between-turns screen and the victory screen.
}

QSize MainWindow::availableScreenSize() const {
  const QScreen* screen = QGuiApplication::primaryScreen();
  const QSize screenSize = screen ? screen->availableGeometry().size() : QSize();

  return QSize(screenSize.width() - BOARD_SCREEN_RESERVED_WIDTH
    , screenSize.height() - BOARD_SCREEN_RESERVED_HEIGHT);
}

bool MainWindow::validateBoardFits() {
  // Area left for the grid once the board margins are removed
  const QSize available = availableScreenSize();
  const QSize gridArea(available.width() - 2 * BOARD_MARGIN
    , available.height() - 2 * BOARD_MARGIN);
  
  // The maze fits if cells can be at least as big as the readable minimum
  if (boardWidget_->calculateCellSize(gridArea) >= BOARD_MIN_CELL_SIZE) {
    return true;
  }

  const QSize grid = boardWidget_->gridSize();  // width = cols, height = row
  QMessageBox::critical(this, TEXT_WINDOW_TITLE
    , QString::fromLatin1(TEXT_ERROR_BOARD_DOES_NOT_FIT).arg(grid.height())
    .arg(grid.width()));

  return false;
}
