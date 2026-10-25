// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <array>
#include <vector>

#include "Maze.hpp"

/**
 * @brief Represents a single cell that is currently visible to a network client.
 */
struct VisibleCell {
  Position position; ///< Grid coordinate of the visible cell.
  Cell contents;     ///< State and contents of the visible cell.
};

/**
 * @brief Represents the public, filtered view of a player for network transmission.
 */
struct PlayerPublicView {
  bool visible = false;       ///< True if this player is within the viewer's field of view.
  Position position{-1, -1};  ///< Coordinates of the player, or {-1, -1} if hidden by fog.
};

/**
 * @brief Sanitized game state snapshot sent to an online client.
 *
 * Excludes hidden board information when Fog of War is enabled to prevent cheating.
 */
struct GameView {
  int rows = 0;               ///< Total rows in the maze grid.
  int cols = 0;               ///< Total columns in the maze grid.
  Position treasure{-1, -1};  ///< Treasure coordinate, or {-1, -1} if obscured by fog.

  std::vector<VisibleCell> visibleCells;    ///< Collection of cells visible to the receiving client.
  std::array<PlayerPublicView, 2> players{}; ///< Public status and positions of both players.

  int currentPlayer = 0;      ///< Index of the player whose turn is active (0 or 1).
  bool gameOver = false;      ///< True if the match has concluded.
  int winner = -1;            ///< Winning player index (0 or 1), or -1 if unresolved.
};
