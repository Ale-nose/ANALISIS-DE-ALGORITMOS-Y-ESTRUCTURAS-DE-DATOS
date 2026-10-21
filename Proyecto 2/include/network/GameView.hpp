// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <array>
#include <vector>

#include "Maze.hpp"

struct VisibleCell {
  Position position;
  Cell contents;
};

struct PlayerPublicView {
  bool visible = false;
  Position position{-1, -1};
};

struct GameView {
  int rows = 0;
  int cols = 0;
  Position treasure{-1, -1};

  std::vector<VisibleCell> visibleCells;
  std::array<PlayerPublicView, 2> players{};

  int currentPlayer = 0;
  bool gameOver = false;
  int winner = -1;
};
