// Copyright Alejandro, Ashley y Kevin
#include "MatchSimulator.hpp"

#include <array>
#include <cstddef>
#include <optional>

#include "AIController.hpp"
#include "Pathfinding.hpp"

namespace {

const std::array<Direction, 4> ALL_DIRECTIONS{
    Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};

/**
 * @brief Direction of the step between two adjacent cells.
 * @param from Starting cell.
 * @param to Adjacent cell.
 * @param direction Receives the direction when the cells are adjacent.
 * @return True if the cells are adjacent.
 */
bool directionBetween(Position from, Position to, Direction& direction) {
  if (to.row == from.row - 1 && to.col == from.col) {
    direction = Direction::UP;
  } else if (to.row == from.row + 1 && to.col == from.col) {
    direction = Direction::DOWN;
  } else if (to.row == from.row && to.col == from.col - 1) {
    direction = Direction::LEFT;
  } else if (to.row == from.row && to.col == from.col + 1) {
    direction = Direction::RIGHT;
  } else {
    return false;
  }
  return true;
}

/**
 * @brief Next move of the player controlled by the simulator: the first step
 * of the cheapest A* path from its cell to the treasure.
 * @param engine Engine of the match.
 * @return A MOVE action (if there is no path, the first legal direction).
 */
Action chooseGuidedAction(const GameEngine& engine) {
  const GameState& state = engine.state();
  const Position from = state.players[state.currentPlayer].position;
  const SearchResult result = findPathAStar(state.maze, from, state.treasure);

  Direction direction = Direction::UP;
  for (std::size_t i = 0; result.found && i + 1 < result.path.size(); ++i) {
    if (result.path[i] == from &&
        directionBetween(from, result.path[i + 1], direction)) {
      return Action{ActionType::MOVE, direction};
    }
  }

  for (Direction candidate : ALL_DIRECTIONS) {
    if (engine.canMove(candidate)) {
      return Action{ActionType::MOVE, candidate};
    }
  }
  return Action{ActionType::MOVE, Direction::UP};
}

}  // namespace

bool playAutomatedMatch(GameEngine& engine, std::mt19937& rng, int maxActions) {
  AIController ai;

  for (int step = 0; step < maxActions && !engine.isGameOver(); ++step) {
    Action action{ActionType::MOVE, Direction::UP};

    if (engine.currentPlayer() == PLAYER_2) {
      action = ai.chooseAction(engine.state(), rng);

      // Same hand-over the window does, so the log has the AI searches
      const std::optional<SearchRecord> record = ai.takeLatestSearchRecord();
      if (record.has_value()) {
        engine.recordAiSearch(*record);
      }
    } else {
      action = chooseGuidedAction(engine);
    }

    if (engine.submitAction(action)) {
      continue;
    }

    // Rejected (for example the AI has no next step): try any legal move so
    // the match never stalls
    bool moved = false;
    for (Direction candidate : ALL_DIRECTIONS) {
      if (engine.submitAction(Action{ActionType::MOVE, candidate})) {
        moved = true;
        break;
      }
    }

    if (!moved) {
      return false;
    }
  }

  return engine.isGameOver();
}
