// Copyright Alejandro, Ashley y Kevin

#include <array>
#include <vector>
#include <random>

#include "AIController.hpp"

Action AIController::chooseAction(const GameState& state, std::mt19937& RNGesus){
  const PlayerState& AI = state.players[PLAYER_2];
  Position AIPos = AI.position;
  int foundAt = -1;
  for (std::size_t i = 0; i < currentPath.size(); ++i) {
    if (currentPath[i] == AIPos){
      foundAt = static_cast<int>(i);
      break;
    }
  }
  if (foundAt == -1) {
    currentPath = recalculatePath(state).path;
    foundAt = 0;
  }
  if (static_cast<std::size_t>(foundAt + 1) >= currentPath.size()) {
    // No next cell: the path is empty or the AI already stands on its last
    // cell. The engine rejects this action without consuming the turn.
    return Action{ActionType::MOVE, Direction::UP};
  }

  if (AI.jumpWalls > 0) {
    std::bernoulli_distribution d(JUMP_WALL_PROB);
    if(d(RNGesus)) {
      Action jump = AIController::chooseRandomJumpWall(state, RNGesus);
      if (jump.type == ActionType::JUMP_WALL) {
        return jump;
      }
    }
  }

  Position nextPos = currentPath[foundAt + 1];
  Direction nextDir = nextDirections(nextPos, AIPos);
  return Action{ActionType::MOVE, nextDir};
}

Direction AIController::nextDirections (Position nextPos, Position currentPos) const {
  int rowDiff = nextPos.row - currentPos.row;
  int colDiff = nextPos.col - currentPos.col;
   if (rowDiff > 0 && colDiff == 0) {
    return Direction::DOWN;

  } else if (rowDiff < 0 && colDiff == 0) {
    return Direction::UP;
  } else if (rowDiff == 0 && colDiff > 0 ) {
    return Direction::RIGHT;
  }

  return Direction::LEFT;
 }


SearchResult AIController::recalculatePath (const GameState& state) {
  return findPathAStar(state.maze, state.players[PLAYER_2].position, state.treasure);
 }

Position AIController::neighbor(Position pos, Direction dir) const{
  switch (dir)
  {
  case Direction::UP : {
    Position newPosition {pos.row-1, pos.col};
    return newPosition;
  }
  case Direction::DOWN : {
    Position newPosition {pos.row+1, pos.col};
    return newPosition;
  }
    case Direction::RIGHT : {
    Position newPosition {pos.row, pos.col + 1};
    return newPosition;
  }
    case Direction::LEFT : {
    Position newPosition {pos.row, pos.col - 1};
    return newPosition;
  }
  }
  return pos ;
}

Action AIController::chooseRandomJumpWall(const GameState& state, std::mt19937& RNGesus) {
  std::array<Direction,4> directions{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
  std::vector<Direction> validDirs;
  
  for(const auto dir: directions ) {
    const PlayerState& AI = state.players[PLAYER_2];
    Position AIPos = AI.position;
    Position target = AIController::neighbor(AIPos, dir);
    if(state.maze.isInside(target) && ! (state.maze.hasPassage(AIPos,target))) {
      validDirs.push_back(dir);
    }
  }
  if(validDirs.empty()) {
    return Action {ActionType::MOVE};
  }
  std::uniform_int_distribution<int> move(0, int(validDirs.size()) - 1);
  return Action {ActionType::JUMP_WALL,validDirs[move(RNGesus)]};
 }
