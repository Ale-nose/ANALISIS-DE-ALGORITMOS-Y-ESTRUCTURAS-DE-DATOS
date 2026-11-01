// Copyright Alejandro, Ashley y Kevin

#pragma once

#include <random>
#include <optional>
#include <vector>

#include "GameState.hpp"
#include "GameAction.hpp"
#include "GameTypes.hpp"
#include "Pathfinding.hpp"

class AIController {
public:
  /**
   * @brief How the AI decides when to spend a Jump Wall.
   *
   * RANDOM spends it on a random valid direction with probability
   * JUMP_WALL_PROB. GREEDY spends it only on the wall that saves the most
   * path cost, and only when the saving reaches AI_JUMP_MIN_GAIN. RANDOM is
   * kept so both policies can be measured against each other on the same
   * seeds.
   */
  enum class JumpWallPolicy {
    RANDOM,
    GREEDY
  };

private:
  /** A Jump Wall worth spending, with the path it opens. */
  struct JumpPlan {
    Direction direction = Direction::UP;
    SearchResult result;
  };

  std::vector<Position> currentPath;
  std::optional<SearchRecord> AIsearchRecord;
  Position lastAIpos = {DEFAULT_COORDINATES,DEFAULT_COORDINATES};
  bool lastActionWasJumpWall = false;
  JumpWallPolicy jumpPolicy = JumpWallPolicy::GREEDY;

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

  /**
   * @brief Stores the record of a search that has already been computed.
   *
   * The record replaces any previous one that has not been collected yet, so
   * only the search the AI acts on is reported. The searches that only
   * evaluate a candidate Jump Wall are deliberately not recorded.
   *
   * @param result Result of the search to report.
   * @param reason Why the search was run.
   */
  void recordSearch(const SearchResult& result, ReplanReason reason);

  /**
   * @brief Chooses an action when A* cannot reach the treasure.
   *
   * Tries three things in order: a Jump Wall that lands closer to the
   * treasure, a step to the open neighbour closest to it, and finally a
   * MOVE the engine rejects when nothing is legal. The second rung skips the
   * cell the AI came from while another neighbour exists, so the AI cannot
   * bounce between two cells for the rest of the match.
   *
   * @param state Current state of the match.
   * @param previousPosition Cell the AI occupied on the previous turn.
   * @return The action to play.
   */
  Action fallbackAction(const GameState& state, Position previousPosition) const;

  /**
   * @brief Looks for a Jump Wall that is worth spending this turn.
   *
   * Compares the cost of the path to the treasure from the current cell
   * against the cost of landing on the far side of each wall and continuing
   * from there. A candidate wins only when it saves at least
   * AI_JUMP_MIN_GAIN, so the AI keeps its Jump Walls for the walls that
   * actually shorten the route.
   *
   * @param state Current state of the match.
   * @return The best candidate with the path it opens, or no value when no
   *         wall saves enough or the treasure is already unreachable.
   */
  std::optional<JumpPlan> planJumpWall(const GameState& state) const;

  /**
   * @brief Gives how much extra path cost a power-up is worth paying for.
   *
   * Each type has its own value because they do not help the same: a Jump
   * Wall is worth what the AI requires a jump to save, and the other two are
   * worth roughly the action they give back.
   *
   * @param type Type of the power-up on the cell.
   * @return The largest detour, in path cost, the AI accepts to collect it.
   */
  int powerUpWorth(PowerUpType type) const;

  /**
   * @brief Looks for a power-up worth a detour on the way to the treasure.
   *
   * Only cells within AI_POWERUP_RADIUS of the AI are considered, which
   * bounds both the number of searches and how far the AI is willing to go
   * off route. A candidate wins when going through it costs no more than the
   * direct path plus what that power-up is worth, and among the candidates
   * the cheapest detour is taken.
   *
   * @param state Current state of the match.
   * @param direct The direct path to the treasure, used as the baseline.
   * @return The joined path through the power-up with its added metrics, or
   *         no value when no detour pays for itself.
   */
  std::optional<SearchResult> planPowerUpDetour(const GameState& state,
                                                const SearchResult& direct) const;

public:

    /**
   * @brief Chooses the action the AI plays on its turn.
   *
   * Follows the path that leads to the treasure, recomputing it when the AI
   * is no longer on it, which happens after a portal, a Jump Wall or a
   * power-up. When A* cannot reach the treasure the AI falls back to
   * fallbackAction instead of standing still. With a Jump Wall available,
   * the current JumpWallPolicy decides whether to spend it. The action is
   * only proposed: the engine validates and applies it.
   *
   * @param state Current state of the match.
   * @param RNGesus Random number generator used for the random choices.
   * @return The action the AI wants to play.
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

  /**
   * @brief Sets how the AI decides to spend its Jump Walls.
   *
   * @param policy The policy to follow from the next turn on.
   */
  inline void setJumpWallPolicy(JumpWallPolicy policy) {
    jumpPolicy = policy;
  }

  ~AIController() = default;
};
