// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QMainWindow>
#include <QSize>

#include "GameState.hpp"

class GameBoardWidget;

/**
 * @brief Main game window. For now it only holds the board and checks that the
 * maze fits on the screen; missing the keyboard, HUD and engine
 */
class MainWindow : public QMainWindow {
 public:
  /**
   * @brief Creates the window with an empty board as its central widget.
   * @param parent Optional parent widget.
   */
  explicit MainWindow(QWidget* parent = nullptr);

  /**
   * @brief Shows a state on the board and sizes the window to fit the screen.
   * @param state Current state of the match.
   */
  void setGameState(const GameState& state);

  /**
   * @brief Checks that every cell of the maze fits on the screen at a
   * readable size.
   * @return True if the maze fits; false if it does not (the caller must end
   * the program, cells are never cut off).
   */
  bool validateBoardFits();

 private:
  /**
   * @brief Screen area the window can use, without the space reserved for the
   * title bar and the HUD.
   * @return Usable width and height in pixels.
   */
  QSize availableScreenSize() const;

  GameBoardWidget* boardWidget_;
};
