// Copyright Alejandro, Ashley y Kevin
#pragma once

// Grid coordinates for maze cells and entities
struct Position {
  int row;
  int col;
};

// Equality operator to compare positions
inline bool operator==(Position a, Position b) {
  return a.row == b.row && a.col == b.col;
}

// Allowed movement and wall orientations
enum class Direction {
  UP,
  DOWN,
  LEFT,
  RIGHT
};

// Maze generation algorithms
enum class GenerationAlgorithm {
  DFS,
  BFS
};

// Match game modes
enum class GameMode {
  SINGLE_PLAYER,
  LOCAL_TWO_PLAYERS,
  ONLINE
};

// Ground terrain types
enum class TerrainType {
  NORMAL,
  MUD
};

// Collectible power-ups
enum class PowerUpType {
  NONE,
  JUMP_WALL,
  DOUBLE_PLAY,
  CONTROL_ENEMY
};

// Special tile mechanics
enum class SpecialCellType {
  NONE,
  SHIELD_TILE
};

// Player action types
enum class ActionType {
  MOVE,
  JUMP_WALL
};
