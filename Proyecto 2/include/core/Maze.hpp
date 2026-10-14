// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstddef>
#include <vector>

#include "Cell.hpp"

class Maze {
 private:
  int rows_;
  int cols_;

  std::vector<Cell> cells_;

  std::size_t indexOf(Position position) const;

 public:
  explicit Maze(int rows, int cols);

  
  void openPassage(Position from, Direction direction);
  
  bool hasPassage(Position from, Position to) const;
  
  std::vector<Position> neighbors(Position position) const;
  
  int countDeadEnds() const;

  inline int entryCost(Position destination) const {
    return cellAt(destination).terrain == TerrainType::MUD ? MUD_COST : 1;
  }
  
  inline Cell& cellAt(Position position) {
    return cells_[indexOf(position)];
  }
  inline const Cell& cellAt(Position position) const {
    return cells_[indexOf(position)];
  }

  inline bool isInside(Position position) const {
    return position.row >= 0 && position.row < rows_ && position.col >= 0 && position.col < cols_;
  }

  inline int rows() const {
    return rows_;
  }

  inline int cols() const {
    return cols_;
  }
};
