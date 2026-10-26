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

  /**
   * @brief Grants the active player one extra action in the current turn.
   *
   * Called when a Double Play power-up is collected. The effect stacks if
   * several are collected, and it must run before finishAction() spends the
   * action of the move that picked the power-up up.
   */
  inline void doublePlay() {
    gameState.actionsLeft++;
  }
  /**
   * @brief Makes the opponent lose their next turn.
   *
   * Called when a Control Enemy power-up is collected by the active player.
   * If the opponent has a shield, one shield is spent and the turn is not
   * lost. Otherwise the opponent is marked to skip their next turn, and
   * beginTurn() clears the mark when that turn would start.
   */
  void applyControlEnemy();
  
   /**
   * @brief Records the extra actions that entering a mud cell costs.
   *
   * Called when the active player enters a mud cell. The move that entered
   * already counts as the first action, so MUD_COST - 1 actions are added to
   * the pending mud actions. They are paid by finishAction() with the actions
   * that remain in the turn, and by beginTurn() in the following turns.
   */
  void applyMudPenalty();

  /**
   * @brief Finds the other cell of a portal pair.
   *
   * Scans the maze for a cell with the same portal identifier that is not
   * the origin cell.
   *
   * @param portalID Identifier shared by both cells of the pair.
   * @param origin Cell of the portal that has just been entered.
   * @return The position of the paired portal, or the position made of
   *         DEFAULT_COORDINATES if there is none.
   */
  Position findPairPortal(int portalID, Position origin) const;

  /**
   * @brief Teleports the active player to the paired portal.
   *
   * Does nothing if the player's cell has no portal or if its pair cannot be
   * found. Otherwise both portals are removed from the maze and the player
   * is moved to the paired cell without spending any extra action. Since the
   * destination portal is removed too, it never triggers a second teleport.
   */
  void activatePortal();

  /**
   * @brief Collects the power-up in the cell of the active player.
   *
   * Jump Wall adds one Jump Wall to the player, Double Play grants an extra
   * action in the current turn and Control Enemy makes the opponent lose the
   * next turn unless they have a shield. A collected power-up is counted in
   * the statistics and removed from the cell, so it cannot be collected
   * twice. Does nothing if the cell has no power-up.
   */
  void collectPowerup();

  /**
   * @brief Collects the shield tile in the cell of the active player.
   *
   * If the cell holds a shield tile, the player gets one shield and the
   * tile is removed from the cell, so it cannot be collected twice.
   */
  void collectShieldTile();

  /**
   * @brief Applies everything that happens when the active player arrives
   *        at a cell.
   *
   * The effects are resolved in a fixed order: portal (at most once),
   * power-up, shield tile, mud penalty and, finally, the victory check. Each
   * step reads the cell where the player stands at that moment, so after a
   * portal the remaining effects come from the destination cell. It must run
   * after the player has moved and before finishAction() is called.
   */
  void resolveCellEntry();

  /**
   * @brief Ends the match if the active player stands on the treasure.
   *
   * Stores the active player as the winner and marks the match as over. It
   * is the last step of resolveCellEntry(), so a portal that lands on the
   * treasure also wins.
   */
  void checkWinner();

  
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

  /**
   * @brief Moves the active player through a wall using one Jump Wall.
   *
   * The jump is accepted only if the match is not over, the player has at
   * least one Jump Wall and the destination is inside the maze. The wall is
   * not opened: it stays closed for later moves. One Jump Wall is spent,
   * even if there was already an open passage between both cells.
   *
   * @param jumpDir Direction of the jump.
   * @return true if the player jumped, false if the action was rejected. A
   *         rejected action does not change the state.
   */
  bool useJumpWall(Direction jumpDir);


  /**
   * @brief Tells whether the match has ended.
   *
   * @return true if a player has reached the treasure, false otherwise.
   */
  inline bool isGameOver() const {
    return gameState.gameOver;
  }

    /**
   * @brief Lists the cells that a player can see under Fog of War.
   *
   * Includes every cell of the maze within FOG_RADIUS of the player, the
   * cell of the player included. It does not check whether fog is enabled:
   * the interface decides whether to apply it. The AI sees the whole maze
   * and does not use this method.
   *
   * @param playerIndex Index of the player whose view is requested.
   * @return The visible cells, in row order.
   */
  std::vector<Position> visibleCells(int playerIndex) const;

};
