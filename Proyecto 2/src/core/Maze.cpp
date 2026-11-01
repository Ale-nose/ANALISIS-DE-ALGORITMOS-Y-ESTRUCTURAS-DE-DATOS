// Copyright Alejandro, Ashley y Kevin
#include "Maze.hpp"

#include <stdexcept>

#include "GameConfig.hpp"

// Converts a direction enum value to its corresponding wall array index
std::size_t wallIndex(Direction direction) {
  return static_cast<std::size_t>(direction);
}

// Returns the opposite cardinal direction for symmetric wall updates
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

// Calculates the resulting coordinate after moving one step in a direction
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

// Constructs a maze with given dimensions and initializes the cell grid
Maze::Maze(int rows, int cols) : rows_(rows), cols_(cols) {
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Maze dimensions must be positive");
  }

  // Allocate contiguous storage for all cells
  cells_.resize(static_cast<std::size_t>(rows_) *
                static_cast<std::size_t>(cols_));
}

// Converts a 2D grid position to a 1D vector index with bounds checking
std::size_t Maze::indexOf(Position position) const {
  if (!isInside(position)) {
    throw std::out_of_range("Position outside maze");
  }

  return static_cast<std::size_t>(position.row) *
             static_cast<std::size_t>(cols_) +
         static_cast<std::size_t>(position.col);
}

// Opens a bidirectional passage between a cell and its adjacent neighbor
void Maze::openPassage(Position from, Direction direction) {
  // Ensure the starting coordinate is valid
  if (!isInside(from)) {
    return;
  }

  const Position to = moved(from, direction);

  // Ensure target cell does not cross outer maze boundaries
  if (!isInside(to)) {
    return;
  }

  // Lower wall from the source cell
  cellAt(from).walls[wallIndex(direction)] = false;

  // Lower opposite wall on target cell to maintain symmetry
  cellAt(to).walls[wallIndex(opposite(direction))] = false;
}

// Verifies if two adjacent cells share an open passage symmetrically
bool Maze::hasPassage(Position from, Position to) const {
  if (!isInside(from) || !isInside(to)) {
    return false;
  }

  Direction direction;

  // Determine relative cardinal direction from 'from' to 'to'
  if (to.row == from.row - 1 && to.col == from.col) {
    direction = Direction::UP;
  } else if (to.row == from.row + 1 && to.col == from.col) {
    direction = Direction::DOWN;
  } else if (to.row == from.row && to.col == from.col - 1) {
    direction = Direction::LEFT;
  } else if (to.row == from.row && to.col == from.col + 1) {
    direction = Direction::RIGHT;
  } else {
    return false;  // Cells are not immediate orthogonal neighbors
  }

  // Both sides of the shared wall must be open
  return !cellAt(from).walls[wallIndex(direction)] &&
         !cellAt(to).walls[wallIndex(opposite(direction))];
}

// Finds all directly accessible neighbor cells through open passages
std::vector<Position> Maze::neighbors(Position position) const {
  std::vector<Position> result;

  if (!isInside(position)) {
    return result;
  }

  // Inspect all 4 cardinal directions for reachable open paths
  for (Direction direction :
       {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
    const Position next = moved(position, direction);

    if (hasPassage(position, next)) {
      result.push_back(next);
    }
  }

  return result;
}

// Counts the number of cells that have only one open exit (cul-de-sacs)
int Maze::countDeadEnds() const {
  int count = 0;

  for (int row = 0; row < rows_; ++row) {
    for (int col = 0; col < cols_; ++col) {
      // A dead end has degree 1 in the passage graph
      if (neighbors({row, col}).size() == 1) {
        ++count;
      }
    }
  }

  return count;
}
