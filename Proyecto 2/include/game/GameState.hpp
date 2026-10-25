// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <array>

#include "GameAction.hpp"
#include "GameConfig.hpp"
#include "GameStats.hpp"
#include "GameTypes.hpp"
#include "Maze.hpp"

// Menu settings used to generate a match
struct GameOptions {
  GameMode mode = GameMode::SINGLE_PLAYER;
  GenerationAlgorithm generator = GenerationAlgorithm::DFS;
  bool fogEnabled = false;
  unsigned int seed = DEFAULT_SEED;
  int rows = MAP_ROWS;
  int cols = MAP_COLS;
  double extraEdgeProb = EXTRA_EDGE_PROB;
  double mudSpawnRate = MUD_SPAWN_RATE;
};

// State of a single player
struct PlayerState {
  Position position{DEFAULT_COORDINATES, DEFAULT_COORDINATES};
  int jumpWalls = 0;
  int shieldCount = 0;
  int pendingMudActions = 0;
  bool skipNextTurn = false;
};

// Single source of truth for a match
struct GameState {
  Maze maze{MAP_ROWS, MAP_COLS};
  std::array<PlayerState, NUM_PLAYERS> players;
  Position treasure{DEFAULT_COORDINATES, DEFAULT_COORDINATES};
  int currentPlayer = PLAYER_1;
  int turnCount = 0;
  int actionsLeft = ACTIONS_PER_TURN;
  int winner = PLAYER_NONE;
  bool gameOver = false;
  GameOptions options;
  GameStats stats;
};
