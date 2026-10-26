// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QMainWindow>
#include <QSize>
#include <memory>
#include <random>

#include "GameAction.hpp"
#include "GameState.hpp"
 
class GameBoardWidget;
class GameEngine;
class HudWidget;
class QKeyEvent;
class QStackedWidget;
class StartWindow;

/**
 * TODO: ver si se puede mostrar la pantalla para pasar al siguiente jugador luego
 * de que se muestre la accion
 */

/**
 * @brief Main game window. It shows the start menu first; when the user
 * presses START it creates the match, hands it to a GameEngine and shows the
 * board. Keyboard input is translated into actions for the engine, and the
 * board is redrawn after every accepted action.
 */
class MainWindow : public QMainWindow {
 public:
  /**
   * @brief Creates the window with the start menu and an empty board.
   * @param parent Optional parent widget.
   */
  explicit MainWindow(QWidget* parent = nullptr);

  /**
   * @brief GameEngine must be a complete type to be destroyed by the unique_ptr.
   */
  ~MainWindow() override;

 protected:
  /**
   * @brief Translates arrows or WASD into a Move action, and the same keys
   * with JUMP_WALL_MODIFIER into a JumpWall action.
   * @param event Key event received by the window.
   */
  void keyPressEvent(QKeyEvent* event) override;

 private:
  /**
   * @brief Builds a new match from the menu options and shows the board.
   * @param options Options chosen in the start menu.
   * @note If the maze cannot be generated the menu stays open with an error 
   * and if it does not fit on the screen the program ends with an error.
   */
  void startMatch(const GameOptions& options);

  /**
   * @brief Sends a human order to the engine and redraws if it was accepted.
   * @param action Move or JumpWall requested by the player.
   * @note A rejected action leaves the player and the turn untouched.
   */
  void submitPlayerAction(const Action& action);

  /**
   * @brief Shows the current engine state again on the board.
   */
  void refreshView();

  /**
   * @brief Tells whether the AI must act now: single player, match not over
   * and it is player 2's turn.
   * @return True if the next action belongs to the AI.
   */
  bool isAiTurn() const;

  /**
   * @brief Programs the next AI action with a timer, so the window keeps
   * responding. Does nothing if it is not the AI's turn.
   */
  void scheduleAiTurn();

  /**
   * @brief Executes one AI action, redraws and schedules the next one while
   * the AI still has the turn.
   */
  void runAiTurn();

  /**
   * @brief Covers the board with the turn handoff screen until the next
   * player presses Ready. Only used in local two-player matches.
   */
  void showTurnHandoff();

  /**
   * @brief Checks that every cell of the maze fits on the screen at a
   * readable size.
   * @return True if the maze fits; false if it does not.
   */
  bool validateBoardFits();

  /**
   * @brief Screen area the window can use, without the space reserved for the
   * title bar and the HUD.
   * @return Usable width and height in pixels.
   */
  QSize availableScreenSize() const;

  // TODO (Ashley): Add NetworkServer and NetworkClient ownership,
  // plus online-mode and connection state needed by MainWindow.
  QStackedWidget* pages_;
  StartWindow* startWindow_;
  QWidget* gamePage_;  // board + hud
  GameBoardWidget* boardWidget_;
  HudWidget* hudWidget_;
  std::unique_ptr<GameEngine> engine_;
  std::mt19937 aiRng_;
};
