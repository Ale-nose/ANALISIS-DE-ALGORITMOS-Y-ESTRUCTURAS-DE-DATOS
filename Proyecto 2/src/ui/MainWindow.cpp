// Copyright Alejandro, Ashley y Kevin
#include "MainWindow.hpp"

#include <QGuiApplication>
#include <QMessageBox>
#include <QScreen>
#include <QString>

#include "BoardTheme.hpp"
#include "GameBoardWidget.hpp"
#include "UiTexts.hpp"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
  setWindowTitle(TEXT_WINDOW_TITLE);
  boardWidget_ = new GameBoardWidget(this);
  setCentralWidget(boardWidget_);
}

void MainWindow::setGameState(const GameState& state) {
  boardWidget_->setGameState(state);

  // Start with the preferred size, but never bigger than the screen.
  resize(boardWidget_->sizeHint().boundedTo(availableScreenSize()));
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

  const QSize grid = boardWidget_->gridSize();  // width = cols, height = rows
  QMessageBox::critical(this, TEXT_WINDOW_TITLE
    , QString::fromLatin1(TEXT_ERROR_BOARD_DOES_NOT_FIT)
        .arg(grid.height()).arg(grid.width()));

  return false;
}
