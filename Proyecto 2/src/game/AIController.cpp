// Copyright Alejandro, Ashley y Kevin

#include <array>
#include <vector>
#include <random>
#include <optional>

#include "AIController.hpp"

Action AIController::chooseAction(const GameState& state, std::mt19937& RNGesus){
  const PlayerState& AI = state.players[PLAYER_2];
  Position AIPos = AI.position;
  Position prevPos = lastAIpos;
  bool cameFromJumpWall = lastActionWasJumpWall;
  lastAIpos = AIPos;
  lastActionWasJumpWall = false;

  int foundAt = -1;
  for (std::size_t i = 0; i < currentPath.size(); ++i) {
    if (currentPath[i] == AIPos){
      foundAt = static_cast<int>(i);
      break;
    }
  }
  if (foundAt == -1) {
    ReplanReason reason = ReplanReason::START;
    if (cameFromJumpWall) {
      reason = ReplanReason::BLOCKED_ROUTE;
    } else if (!currentPath.empty() && manhattanDistance(AIPos, prevPos) != 1) {
      reason = ReplanReason::PORTAL;
    } else if (!currentPath.empty()) {
      reason = ReplanReason::BLOCKED_ROUTE;
    }

    SearchResult replan = recalculatePath(state, reason);
    if (!replan.found) {
      // The treasure cannot be reached through open passages so the ai uses the fallback
      currentPath.clear();
      return fallbackAction(state, prevPos);
    }
    std::optional<SearchResult> detour = planPowerUpDetour(state, replan);
    if (detour.has_value()) {
      recordSearch(detour.value(), ReplanReason::POWER_UP);
      currentPath = detour.value().path;
    } else {
      currentPath = replan.path;
    }
    foundAt = 0;
  }

  if (AI.jumpWalls > 0) {
    if (jumpPolicy == JumpWallPolicy::GREEDY) {
      std::optional<JumpPlan> plan = planJumpWall(state);
      if (plan.has_value()) {
        // The path from the far side of the wall becomes the new plan to avoid recalculating it
        recordSearch(plan.value().result, ReplanReason::BLOCKED_ROUTE);
        currentPath = plan.value().result.path;
        lastActionWasJumpWall = true;
        return Action{ActionType::JUMP_WALL, plan.value().direction};
      }
    } else {
      std::bernoulli_distribution d(JUMP_WALL_PROB);
      if(d(RNGesus)) {
        Action jump = AIController::chooseRandomJumpWall(state, RNGesus);
        if (jump.type == ActionType::JUMP_WALL) {
          lastActionWasJumpWall = true;
          return jump;
        }
      }
    }
  }

  if (static_cast<std::size_t>(foundAt + 1) >= currentPath.size()) {
    //the ai already stands on the last cell of its path
    return fallbackAction(state, prevPos);
  }

  Position nextPos = currentPath[foundAt + 1];
  Direction nextDir = nextDirections(nextPos, AIPos);
  return Action{ActionType::MOVE, nextDir};
}

Action AIController::fallbackAction(const GameState& state, Position previousPosition) const {
  const std::array<Direction,4> directions{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
  const PlayerState& AI = state.players[PLAYER_2];
  const Position AIPos = AI.position;
  const Position goal = state.treasure;

  if (AI.jumpWalls > 0) {
    bool jumpFound = false;
    int bestJumpDistance = manhattanDistance(AIPos, goal);
    Direction bestJumpDir = Direction::UP;
    for (const auto dir : directions) {
      const Position target = AIController::neighbor(AIPos, dir);
      if (!state.maze.isInside(target) || state.maze.hasPassage(AIPos, target)) {
        continue;
      }
      const int distance = manhattanDistance(target, goal);
      if (distance < bestJumpDistance) {
        bestJumpDistance = distance;
        bestJumpDir = dir;
        jumpFound = true;
      }
    }
    if (jumpFound) {
      return Action{ActionType::JUMP_WALL, bestJumpDir};
    }
  }

  const std::vector<Position> openNeighbors = state.maze.neighbors(AIPos);
  bool stepFound = false;
  int bestStepDistance = 0;
  Position bestStep = AIPos;
  for (const Position& candidate : openNeighbors) {
    if (candidate == previousPosition && openNeighbors.size() > 1) {
      continue;
    }
    const int distance = manhattanDistance(candidate, goal);
    if (!stepFound || distance < bestStepDistance) {
      bestStepDistance = distance;
      bestStep = candidate;
      stepFound = true;
    }
  }
  if (stepFound) {
    return Action{ActionType::MOVE, nextDirections(bestStep, AIPos)};
  }

  // Nothing is legal. The engine rejects this action without consuming the turn.
  return Action{ActionType::MOVE, Direction::UP};
}

std::optional<AIController::JumpPlan> AIController::planJumpWall(const GameState& state) const {
  const std::array<Direction,4> directions{Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
  const Position AIPos = state.players[PLAYER_2].position;
  const Position goal = state.treasure;

  const SearchResult direct = findPathAStar(state.maze, AIPos, goal);
  if (!direct.found) {
    return std::nullopt;
  }

  std::optional<JumpPlan> best;
  int bestCost = direct.metrics.pathCost - AI_JUMP_MIN_GAIN;

  for (const auto dir : directions) {
    const Position target = AIController::neighbor(AIPos, dir);
    if (!state.maze.isInside(target) || state.maze.hasPassage(AIPos, target)) {
      continue;
    }
    const SearchResult fromTarget = findPathAStar(state.maze, target, goal);
    if (!fromTarget.found) {
      continue;
    }
    const int totalCost = state.maze.entryCost(target) + fromTarget.metrics.pathCost;
    if (totalCost <= bestCost) {
      bestCost = totalCost;
      JumpPlan plan;
      plan.direction = dir;
      plan.result = fromTarget;
      best = plan;
    }
  }
  return best;
}

int AIController::powerUpWorth(PowerUpType type) const {
  switch (type) {
    case PowerUpType::JUMP_WALL: {
      return AI_WORTH_JUMP_WALL;
    }
    case PowerUpType::DOUBLE_PLAY: {
      return AI_WORTH_DOUBLE_PLAY;
    }
    case PowerUpType::CONTROL_ENEMY: {
      return AI_WORTH_CONTROL_ENEMY;
    }
    case PowerUpType::NONE: {
      return 0;
    }
  }
  return 0;
}

std::optional<SearchResult> AIController::planPowerUpDetour(const GameState& state,
                                                            const SearchResult& direct) const {
  const Position AIPos = state.players[PLAYER_2].position;
  const Position goal = state.treasure;

  std::optional<SearchResult> best;
  int bestOverhead = 0;

  for (int row = 0; row < state.maze.rows(); ++row) {
    for (int col = 0; col < state.maze.cols(); ++col) {
      const Position cell{row, col};
      const PowerUpType type = state.maze.cellAt(cell).powerUp;
      if (type == PowerUpType::NONE || cell == AIPos || cell == goal) {
        continue;
      }
      if (manhattanDistance(AIPos, cell) > AI_POWERUP_RADIUS) {
        continue;
      }
      const SearchResult toPowerUp = findPathAStar(state.maze, AIPos, cell);
      if (!toPowerUp.found || toPowerUp.path.empty()) {
        continue;
      }
      const SearchResult toGoal = findPathAStar(state.maze, cell, goal);
      if (!toGoal.found || toGoal.path.empty()) {
        continue;
      }
      const int overhead = toPowerUp.metrics.pathCost + toGoal.metrics.pathCost
                         - direct.metrics.pathCost;
      if (overhead > powerUpWorth(type)) {
        continue;
      }
      if (best.has_value() && overhead >= bestOverhead) {
        continue;
      }

      SearchResult joined;
      joined.found = true;
      joined.path = toPowerUp.path;
      joined.path.insert(joined.path.end(), toGoal.path.begin() + 1, toGoal.path.end());
      joined.metrics.pathCost = toPowerUp.metrics.pathCost + toGoal.metrics.pathCost;
      joined.metrics.visitedNodes = toPowerUp.metrics.visitedNodes + toGoal.metrics.visitedNodes;
      joined.metrics.cpuTimeMs = toPowerUp.metrics.cpuTimeMs + toGoal.metrics.cpuTimeMs;
      bestOverhead = overhead;
      best = joined;
    }
  }
  return best;
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


void AIController::recordSearch(const SearchResult& result, ReplanReason reason) {
  SearchRecord tempRecord;
  tempRecord.algorithm = SearchAlgorithm::A_STAR;
  tempRecord.reason = reason;
  tempRecord.metrics = result.metrics;
  AIsearchRecord.emplace(tempRecord);
}

SearchResult AIController::recalculatePath (const GameState& state,ReplanReason reason) {
  SearchResult result = findPathAStar(state.maze, state.players[PLAYER_2].position, state.treasure);
  AIController::recordSearch(result, reason);
  return result;
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


std::optional<SearchRecord> AIController::takeLatestSearchRecord() {
  if (AIsearchRecord.has_value()) {
    SearchRecord record = AIsearchRecord.value();
    AIsearchRecord.reset();
    return record;
  }
return std::nullopt;
}
