// Copyright Alejandro, Ashley y Kevin

#include <utility>

#include "GameEngine.hpp"

GameEngine::GameEngine(GameState gameStateArg): gameState(std::move(gameStateArg))
{ 
}

Position GameEngine::newPos(Position pos, Direction dir) const {
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

bool GameEngine::submitAction(const Action& action){
  switch (action.type)
  {
  case ActionType::MOVE :{
    return movePlayer(action.direction);

  } case ActionType::JUMP_WALL : {
    return false;
  }
  
  }

  return false;
}

bool GameEngine::canMove(Direction directionToCheck) const {
  if (this->gameState.gameOver) {
    return false;
  }

  Position current  = this->gameState.players[this->gameState.currentPlayer].position;
  Position destiny  = newPos(current, directionToCheck);
  return (this->gameState.maze.isInside(destiny) && this->gameState.maze.hasPassage(current,destiny));
}

bool GameEngine::movePlayer(Direction directionToMove) {
  if (!canMove(directionToMove)) {
    return false;
  }
  this->gameState.players[this->gameState.currentPlayer].position = newPos(this->gameState.players
    [this->gameState.currentPlayer].position,directionToMove);
  return true;
}

const GameState& GameEngine::state() const {

  return this->gameState;
}