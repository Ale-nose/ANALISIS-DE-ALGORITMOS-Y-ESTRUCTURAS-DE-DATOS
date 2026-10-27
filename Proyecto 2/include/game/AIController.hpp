// Copyright Alejandro, Ashley y Kevin

#pragma once

#include <random>
#include <optional>

#include "GameState.hpp"
#include "GameAction.hpp"
#include "GameTypes.hpp"
#include "Pathfinding.hpp"

class AIController {
private:
  std::vector<Position> currentPath;
  std::optional<SearchRecord> AIsearchRecord;
  Position lastAIpos = {DEFAULT_COORDINATES,DEFAULT_COORDINATES};

  /**
   * @brief Gives the direction of the step between two adjacent cells.
   *
   * Row 0 is the top row, so a larger row means DOWN and a smaller one means
   * UP. It is meant for consecutive cells of a path, which are always
   * adjacent.
   *
   * @param nextPos Cell the AI wants to reach.
   * @param currentPos Cell where the AI stands.
   * @return The direction that leads from currentPos to nextPos.
   */
  Direction nextDirections(Position nextPos, Position currentPos) const;

  /**
   * @brief Gives the cell next to a position in a given direction.
   *
   * Row 0 is the top row, so UP decreases the row and DOWN increases it. The
   * result is not checked against the maze bounds.
   *
   * @param pos Starting cell.
   * @param dir Direction of the step.
   * @return The adjacent cell in that direction.
   */
  Position neighbor(Position pos, Direction dir) const;
public:

    /**
   * @brief Chooses the action the AI plays on its turn.
   *
   * Follows the path that leads to the treasure, recomputing it when the AI
   * is no longer on it, which happens after a portal, a Jump Wall or a
   * power-up. With a Jump Wall available, the AI may spend it on a random
   * valid direction with probability JUMP_WALL_PROB. The action is only
   * proposed: the engine validates and applies it.
   *
   * @param state Current state of the match.
   * @param RNGesus Random number generator used for the random choices.
   * @return The action the AI wants to play. If there is no step to take,
   *         the returned action is rejected by the engine.
   */
  Action chooseAction(const GameState& state, std::mt19937& RNGesus);

   /**
   * @brief Computes the cheapest path from the AI to the treasure with A*.
   *
   * The search is recorded so the game log can report it. The record is kept
   * until takeLatestSearchRecord() collects it.
   *
   * @param state Current state of the match.
   * @param reason Why the path is being recomputed.
   * @return The search result, with the path and its metrics.
   */
  SearchResult recalculatePath(const GameState& state, ReplanReason reason);

  /**
   * @brief Picks a random direction to spend a Jump Wall on.
   *
   * A direction is valid when the destination is inside the maze and there
   * is a wall between both cells, so the jump is never wasted where the AI
   * could simply walk.
   *
   * @param state Current state of the match.
   * @param RNGesus Random number generator used to pick the direction.
   * @return A JUMP_WALL action, or a MOVE action when no direction is valid.
   */
  Action chooseRandomJumpWall(const GameState& state, std::mt19937& RNGesus);

    /**
   * @brief Hands over the record of the last search, if there is one.
   *
   * The record is removed, so the same search is never reported twice. The
   * caller passes it to GameEngine::recordAiSearch.
   *
   * @return The record of the last search, or no value when no search has
   *         happened since the previous call.
   */
  std::optional<SearchRecord> takeLatestSearchRecord();


  ~AIController() = default;
};



