// Copyright Alejandro, Ashley y Kevin
#pragma once

struct Position {
  int row;
  int col;
};

bool operator==(Position a, Position b) {
  return a.row == b.row && a.col == b.col;
}

enum class Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

enum class GenerationAlgorithm {
  DFS,
  BFS
};

enum class GameMode {
  SINGLE_PLAYER,
  LOCAL_TWO_PLAYERS,
  ONLINE
};

enum class TerrainType {
  NORMAL,
  MUD
};

enum class PowerUpType {
  NONE,
  JUMP_WALL,
  DOUBLE_PLAY,
  CONTROL_ENEMY
};

enum class SpecialCellType {
  NONE,
  SHIELD_TILE
};

enum class ActionType {
  MOVE,
  JUMP_WALL
};
