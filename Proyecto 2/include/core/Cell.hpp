// Copyright Alejandro, Ashley y Kevin
#pragma once
#include <array>

#include "GameConfig.hpp"
#include "GameTypes.hpp"

// Represents a single cell (node) in the maze.
struct Cell {
  TerrainType terrain = TerrainType::NORMAL;  // Ground type (Normal or Mud)
  PowerUpType powerUp = PowerUpType::NONE;    // Collectible power-up
  SpecialCellType special = SpecialCellType::NONE;  // Special tile
  int portalID = PORTAL_ID;  // Portal pair ID (-1 if none)
  std::array<bool, 4> walls = {true, true, true,
                               true};  // Up, Down, Left, Right walls
};
