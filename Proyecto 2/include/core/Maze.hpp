// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstddef>
#include <vector>

#include "Cell.hpp"
#include "GameConfig.hpp"

/**
 * @brief Represents the maze grid and its underlying graph structure.
 */
class Maze {
 private:
  int rows_; ///< Total number of rows in the maze.
  int cols_; ///< Total number of columns in the maze.

  std::vector<Cell> cells_; ///< Flat storage of all cells in row-major order.

  /**
   * @brief Computes the 1D index in cells_ corresponding to a 2D position.
   * @param position Grid coordinates.
   * @return Linear index in the cells vector.
   */
  std::size_t indexOf(Position position) const;

 public:
  /**
   * @brief Constructs a maze with all boundary walls closed.
   * @param rows Total number of rows.
   * @param cols Total number of columns.
   */
  explicit Maze(int rows, int cols);

  /**
   * @brief Opens a passage between a cell and its neighbor in the given direction.
   * @param from Origin cell coordinate.
   * @param direction Direction towards the neighbor cell.
   */
  void openPassage(Position from, Direction direction);
  
  /**
   * @brief Checks if a direct open passage exists between two adjacent cells.
   * @param from First cell coordinate.
   * @param to Second cell coordinate.
   * @return true if passage is open, false otherwise.
   */
  bool hasPassage(Position from, Position to) const;
  
  /**
   * @brief Retrieves all accessible neighboring coordinates without crossing walls.
   * @param position Cell coordinate to inspect.
   * @return Vector of accessible neighboring positions.
   */
  std::vector<Position> neighbors(Position position) const;
  
  /**
   * @brief Counts the number of dead-end cells in the maze.
   * @return Number of cells with exactly one open connection.
   */
  int countDeadEnds() const;

  /**
   * @brief Computes the action cost required to enter a destination cell.
   * @param destination Target coordinate.
   * @return Action cost (MUD_COST if mud terrain, NORMAL_COST otherwise).
   */
  inline int entryCost(Position destination) const {
    return cellAt(destination).terrain == TerrainType::MUD ? MUD_COST : NORMAL_COST;
  }
  
  /**
   * @brief Retrieves a mutable reference to the cell at the given position.
   * @param position Target coordinate.
   * @return Reference to the Cell object.
   */
  inline Cell& cellAt(Position position) {
    return cells_[indexOf(position)];
  }

  /**
   * @brief Retrieves a read-only reference to the cell at the given position.
   * @param position Target coordinate.
   * @return Const reference to the Cell object.
   */
  inline const Cell& cellAt(Position position) const {
    return cells_[indexOf(position)];
  }

  /**
   * @brief Verifies whether a coordinate lies within the maze boundaries.
   * @param position Target coordinate.
   * @return true if the position is within bounds, false otherwise.
   */
  inline bool isInside(Position position) const {
    return position.row >= 0 && position.row < rows_ && position.col >= 0 && position.col < cols_;
  }

  /**
   * @brief Gets the total row count of the maze.
   * @return Number of rows.
   */
  inline int rows() const {
    return rows_;
  }

  /**
   * @brief Gets the total column count of the maze.
   * @return Number of columns.
   */
  inline int cols() const {
    return cols_;
  }
};
