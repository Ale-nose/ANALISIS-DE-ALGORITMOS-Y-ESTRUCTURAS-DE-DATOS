// CopyRIGHT Alejandro, Ashley y Kevin
#include <stdexcept>

#include "Maze.hpp"
#include "GameConfig.hpp"

std::size_t wallIndex(Direction direction) {
  return static_cast<std::size_t>(direction);
}

Direction opposite(Direction direction) {
  switch (direction) {
    case Direction::UP:
      return Direction::DOWN;

    case Direction::DOWN:
      return Direction::UP;

    case Direction::LEFT:
      return Direction::RIGHT;

    case Direction::RIGHT:
      return Direction::LEFT;
  }

  throw std::invalid_argument("Invalid direction");
}

Position moved(Position position, Direction direction) {
  switch (direction) {
    case Direction::UP:
      --position.row;
      break;

    case Direction::DOWN:
      ++position.row;
      break;

    case Direction::LEFT:
      --position.col;
      break;

    case Direction::RIGHT:
      ++position.col;
      break;

    default:
      throw std::invalid_argument("Invalid direction");
  }

  return position;
}

Maze::Maze(int rows, int cols) : rows_(rows), cols_(cols) {
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Maze dimensions must be positive");
  }

  cells_.resize(static_cast<std::size_t>(rows_) * static_cast<std::size_t>(cols_));
}

std::size_t Maze::indexOf(Position position) const {
  if (!isInside(position)) {
    throw std::out_of_range("Position outside maze");
  }

  return static_cast<std::size_t>(position.row) * static_cast<std::size_t>(cols_) +
    static_cast<std::size_t>(position.col);
}

void Maze::openPassage(Position from, Direction direction) {
  if (!isInside(from)) {
    return;
  }

  const Position to = moved(from, direction);

  if (!isInside(to)) {
    return;
  }

  cellAt(from).walls[wallIndex(direction)] = false;

  cellAt(to).walls[wallIndex(opposite(direction))] = false;
}

bool Maze::hasPassage(Position from, Position to) const {
  if (!isInside(from) || !isInside(to)) {
    return false;
  }

  Direction direction;

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

  return !cellAt(from).walls[wallIndex(direction)] && 
    !cellAt(to).walls[wallIndex(opposite(direction))];
}

std::vector<Position> Maze::neighbors(Position position) const {
  std::vector<Position> result;

  if (!isInside(position)) {
    return result;
  }

  for (Direction direction : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
    const Position next = moved(position, direction);

    if (hasPassage(position, next)) {
      result.push_back(next);
    }
  }

  return result;
}

int Maze::countDeadEnds() const {
  int count = 0;

  for (int row = 0; row < rows_; ++row) {
    for (int col = 0; col < cols_; ++col) {
      if (neighbors({row, col}).size() == 1) {
        ++count;
      }
    }
  }

  return count;
}
