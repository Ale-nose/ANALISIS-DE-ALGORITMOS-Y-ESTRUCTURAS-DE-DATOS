// Copyright Alejandro, Ashley y Kevin

#include "WorldSpawner.hpp"

void WorldSpawner::placeMud(Maze& maze, Position posPlayer1,
                            Position posPlayer2, double rate,
                            std::mt19937& random) {
  std::bernoulli_distribution chance(rate);
  for (int i = 0; i < maze.rows(); i++) {
    for (int j = 0; j < maze.cols(); j++) {
      Position current{i, j};
      if (current == posPlayer1 || current == posPlayer2) {
        // skips it
      } else {
        if (chance(random)) {
          maze.cellAt(current).terrain = TerrainType::MUD;
        }
      }
    }
  }
}

void WorldSpawner::placeShieldTile(Maze& maze, Position posPlayer1,
                                   Position posPlayer2, Position treasure,
                                   std::mt19937& random) {
  std::bernoulli_distribution chance(SHIELD_SPAWN_RATE);
  for (int i = 0; i < maze.rows(); i++) {
    for (int j = 0; j < maze.cols(); j++) {
      Position current{i, j};
      if (current == posPlayer1 || current == posPlayer2 ||
          current == treasure) {
        // skips it
      } else {
        if (chance(random)) {
          maze.cellAt(current).special = SpecialCellType::SHIELD_TILE;
        }
      }
    }
  }
}

void WorldSpawner::placePowerUp(Maze& maze, Position posPlayer1,
                                Position posPlayer2, Position treasure,
                                std::mt19937& random) {
  std::array<PowerUpType, 3> powerPool{PowerUpType::CONTROL_ENEMY,
                                       PowerUpType::DOUBLE_PLAY,
                                       PowerUpType::JUMP_WALL};
  std::bernoulli_distribution chance(POWER_SPAWN_RATE);
  std::uniform_int_distribution<int> power(0, static_cast<int>(powerPool.size()) - 1);
  for (int i = 0; i < maze.rows(); i++) {
    for (int j = 0; j < maze.cols(); j++) {
      Position current{i, j};
      if (current == posPlayer1 || current == posPlayer2 ||
          current == treasure) {
        // skips it
      } else {
        if (chance(random)) {
          maze.cellAt(current).powerUp = powerPool[power(random)];
        }
      }
    }
  }
}

void WorldSpawner::placePortal(Maze& maze, Position posPlayer1,
                               Position posPlayer2, Position treasure,
                               std::mt19937& random) {
  int nextID = 1;
  std::bernoulli_distribution chance(PORTAL_SPAWN_RATE);
  std::vector<Position> posiblePos;
  for (int i = 0; i < maze.rows(); i++) {
    for (int j = 0; j < maze.cols(); j++) {
      Position current{i, j};
      if (current == posPlayer1 || current == posPlayer2 ||
          current == treasure) {
        // skips it
      } else {
        if (chance(random)) {
          posiblePos.emplace_back(current);  // stored for shuffling
        }
      }
    }
  }
  std::shuffle(posiblePos.begin(), posiblePos.end(), random);
  for (int i = 0; i + 1 < static_cast<int>(posiblePos.size()); i += 2) {
    maze.cellAt(posiblePos[i]).portalID = nextID;
    maze.cellAt(posiblePos[i + 1]).portalID = nextID;
    nextID++;
  }
}
