// Copyright Alejandro, Ashley y Kevin

#include <random>
#include <stdexcept>
#include <string>

#include "AIController.hpp"

namespace {

/** Fails the test by throwing, which is what TestMain.cpp reports. */
void check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

/** Builds an empty maze of the given size with every wall closed. */
GameState makeState(int rows, int cols) {
  GameState state;
  state.maze = Maze(rows, cols);
  return state;
}

/** Opens a straight corridor along a row, from column 0 to lastCol. */
void openRow(GameState& state, int row, int lastCol) {
  for (int col = 0; col < lastCol; ++col) {
    state.maze.openPassage(Position{row, col}, Direction::RIGHT);
  }
}

}  // namespace

/**
 * @brief The AI must not stall when A* cannot reach the treasure.
 *
 * The treasure sits behind a wall with no passage leading to it. Before the
 * contingency ladder the AI returned a rejected MOVE every turn and the
 * match never ended. It must now spend a Jump Wall towards the treasure.
 */
void testAiFallbackJump() {
  GameState state = makeState(5, 5);
  state.players[PLAYER_2].position = Position{0, 0};
  state.players[PLAYER_2].jumpWalls = 1;
  state.treasure = Position{0, 1};

  AIController ai;
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::JUMP_WALL,
        "Without a path the AI should jump instead of stalling");
  check(action.direction == Direction::RIGHT,
        "The jump should go towards the treasure");
}

/**
 * @brief The AI must still move when it has no Jump Walls left.
 *
 * Same unreachable treasure, no Jump Walls. The second rung of the ladder
 * has to step to the only open neighbour.
 */
void testAiFallbackStep() {
  GameState state = makeState(5, 5);
  state.maze.openPassage(Position{0, 0}, Direction::DOWN);
  state.players[PLAYER_2].position = Position{0, 0};
  state.players[PLAYER_2].jumpWalls = 0;
  state.treasure = Position{0, 4};

  AIController ai;
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE && action.direction == Direction::DOWN,
        "Without a path and without Jump Walls the AI should walk");
}

/**
 * @brief The fallback must not bounce between two cells.
 *
 * After stepping to its only neighbour the AI has two open cells to choose
 * from, and it has to pick the one it did not come from.
 */
void testAiNoOscillation() {
  GameState state = makeState(5, 5);
  state.maze.openPassage(Position{0, 0}, Direction::DOWN);
  state.maze.openPassage(Position{1, 0}, Direction::DOWN);
  state.players[PLAYER_2].position = Position{0, 0};
  state.treasure = Position{0, 4};

  AIController ai;
  std::mt19937 rng(42);
  ai.chooseAction(state, rng);
  state.players[PLAYER_2].position = Position{1, 0};
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE && action.direction == Direction::DOWN,
        "The AI should not step back into the cell it came from");
}

/**
 * @brief A Jump Wall that saves enough cost must be spent.
 *
 * The way around costs 6 and jumping the wall costs 2, so the saving of 4
 * clears AI_JUMP_MIN_GAIN.
 */
void testAiGreedyJump() {
  GameState state = makeState(5, 5);
  state.maze.openPassage(Position{0, 0}, Direction::DOWN);
  state.maze.openPassage(Position{1, 0}, Direction::DOWN);
  state.maze.openPassage(Position{2, 0}, Direction::RIGHT);
  state.maze.openPassage(Position{2, 1}, Direction::RIGHT);
  state.maze.openPassage(Position{2, 2}, Direction::UP);
  state.maze.openPassage(Position{1, 2}, Direction::UP);
  state.maze.openPassage(Position{0, 1}, Direction::RIGHT);
  state.players[PLAYER_2].position = Position{0, 0};
  state.players[PLAYER_2].jumpWalls = 1;
  state.treasure = Position{0, 2};

  AIController ai;
  ai.setJumpWallPolicy(AIController::JumpWallPolicy::GREEDY);
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::JUMP_WALL && action.direction == Direction::RIGHT,
        "The AI should spend a Jump Wall when the saving clears the threshold");
}

/**
 * @brief A Jump Wall that saves nothing must be kept.
 *
 * The corridor is already open, so no wall shortens the route.
 */
void testAiKeepsJump() {
  GameState state = makeState(3, 5);
  openRow(state, 0, 4);
  state.players[PLAYER_2].position = Position{0, 0};
  state.players[PLAYER_2].jumpWalls = 1;
  state.treasure = Position{0, 4};

  AIController ai;
  ai.setJumpWallPolicy(AIController::JumpWallPolicy::GREEDY);
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE && action.direction == Direction::RIGHT,
        "The AI should keep a Jump Wall that would save nothing");
}

/**
 * @brief A power-up one step off the route is worth the detour.
 *
 * Going through it costs 2 more than the direct path, which is what a
 * Double Play is worth.
 */
void testAiPowerUpDetour() {
  GameState state = makeState(3, 5);
  openRow(state, 0, 4);
  state.maze.openPassage(Position{0, 0}, Direction::DOWN);
  state.maze.cellAt(Position{1, 0}).powerUp = PowerUpType::DOUBLE_PLAY;
  state.players[PLAYER_2].position = Position{0, 0};
  state.treasure = Position{0, 4};

  AIController ai;
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE && action.direction == Direction::DOWN,
        "The AI should detour for a power-up that costs what it is worth");
}

/**
 * @brief A power-up too far off the route must be ignored.
 *
 * The same Double Play three cells away costs more than it gives back.
 */
void testAiPowerUpRejected() {
  GameState state = makeState(5, 5);
  openRow(state, 0, 4);
  state.maze.openPassage(Position{0, 0}, Direction::DOWN);
  state.maze.openPassage(Position{1, 0}, Direction::DOWN);
  state.maze.openPassage(Position{2, 0}, Direction::DOWN);
  state.maze.cellAt(Position{3, 0}).powerUp = PowerUpType::DOUBLE_PLAY;
  state.players[PLAYER_2].position = Position{0, 0};
  state.treasure = Position{0, 4};

  AIController ai;
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE && action.direction == Direction::RIGHT,
        "The AI should ignore a power-up whose detour does not pay for itself");
}

/**
 * @brief The random policy stays available so both can be compared.
 *
 * BenchmarkRunner switches between the two on the same seeds to measure what
 * the greedy policy is worth.
 */
void testAiRandomPolicy() {
  GameState state = makeState(3, 5);
  openRow(state, 0, 4);
  state.players[PLAYER_2].position = Position{0, 0};
  state.players[PLAYER_2].jumpWalls = 1;
  state.treasure = Position{0, 4};

  AIController ai;
  ai.setJumpWallPolicy(AIController::JumpWallPolicy::RANDOM);
  std::mt19937 rng(42);
  Action action = ai.chooseAction(state, rng);

  check(action.type == ActionType::MOVE || action.type == ActionType::JUMP_WALL,
        "The random policy should still produce a legal action");
}