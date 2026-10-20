// Copyright Alejandro, Ashley y Kevin
#pragma once
#include <array>
#include "GameTypes.hpp"
#include "GameConfig.hpp"

struct Cell {
  TerrainType terrain = TerrainType::NORMAL;
  PowerUpType powerUp = PowerUpType::NONE;
  SpecialCellType special = SpecialCellType::NONE;

  int portalID = PORTAL_ID;

  std::array<bool, 4> walls = {true, true, true, true};
};
