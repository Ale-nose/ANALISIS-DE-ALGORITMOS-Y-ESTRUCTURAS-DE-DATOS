// Copyright Alejandro, Ashley y Kevin

#pragma once

#include "GameState.hpp"
#include "GameAction.hpp"
#include "GameTypes.hpp"




class GameEngine
{
private:
  /**
   * @brief Computes the cell next to a position in a given direction.
   *
   * Row 0 is the top row of the maze, so UP decreases the row and DOWN
   * increases it. The result is not checked against the maze bounds.
   *
   * @param pos Starting cell.
   * @param dir Direction of the step.
   * @return The adjacent cell in that direction.
   */
  Position newPos (Position pos, Direction dir) const ;
  GameState gameState;

  /**
   * @brief Prepares the turn of the player that has just received it.
   *
   * If the player is marked to lose the turn, the mark is cleared and the
   * turn passes to the other player. Otherwise, the pending mud actions are
   * paid one by one with the actions available this turn, and the turn passes
   * if none are left. Mud actions that cannot be paid stay pending for the
   * next turn.
   */
  void beginTurn();

  /**
   * @brief Consumes one action of the active player after an accepted move.
   *
   * If actions remain (for example after a Double Play), the same player
   * acts again. When none remain, the turn counters are updated and, if the
   * match is not over, the turn passes to the other player.
   */
  void finishAction();

  /**
   * @brief Passes the turn to the other player.
   *
   * Switches the active player, gives them the actions of a regular turn
   * and then lets beginTurn() resolve lost turns and pending mud actions.
   */
  void advanceTurn();

  
  public:
  /**
   * @brief Creates an engine that manages the given match.
   *
   * @param gameStateArg Initial state, moved into the engine.
   */
  explicit GameEngine(GameState gameStateArg);

  ~GameEngine() = default;
  
  /**
   * @brief Executes one action requested by a player or by the AI.
   *
   * @param action Action to execute.
   * @return true if the action was accepted and applied, false if it was
   *         rejected. A rejected action does not change the state.
   */
  bool submitAction(const Action& action);

  /**
   * @brief Checks whether the active player can step in a direction.
   *
   * The move is legal if the match is not over, the destination is inside
   * the maze and there is an open passage between both cells.
   *
   * @param directionToCheck Direction of the step.
   * @return true if the move is legal, false otherwise.
   */
  bool canMove(Direction directionToCheck) const;

  /**
   * @brief Moves the active player one cell in a direction.
   *
   * The move is validated with canMove() first. Only the position of the
   * active player changes.
   *
   * @param directionToMove Direction of the step.
   * @return true if the player moved, false if the move was illegal.
   */
  bool movePlayer(Direction directionToMove);
  
  /**
   * @brief Gives access to the current state of the match.
   *
   * @return Reference to the game state.
   */
  const GameState& state() const;

  /**
   * @brief Gives the index of the player whose turn it is.
   *
   * @return PLAYER_1 or PLAYER_2.
   */
  inline int currentPlayer() const {
    return gameState.currentPlayer;
  }
};