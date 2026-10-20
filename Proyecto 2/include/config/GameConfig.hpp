// Copyright Alejandro, Ashley y Kevin
#pragma once

// Maze Dimensions
inline constexpr int MAP_ROWS = 10;
inline constexpr int MAP_COLS = 20;

// Movement costs
inline constexpr int NORMAL_COST = 1;
inline constexpr int MUD_COST = 2;

// Fog of War
inline constexpr int FOG_RADIUS = 3;

// Generation probabilities
inline constexpr double EXTRA_EDGE_PROB = 0.10;

// Spawn probabilities
inline constexpr double POWER_SPAWN_RATE = 0.08;
inline constexpr double PORTAL_SPAWN_RATE = 0.03;
inline constexpr double MUD_SPAWN_RATE = 0.12;
inline constexpr double SHIELD_SPAWN_RATE = 0.02;

// Player id 
inline constexpr int PLAYER_1 = 0;
inline constexpr int PLAYER_2 = 1;
inline constexpr int PLAYER_NONE = -1;

// Default coordinates
inline constexpr int DEFAULT_COORDINATES = -1;

inline constexpr int NUM_PLAYERS = 2;
inline constexpr int ACTIONS_PER_TURN = 1;
inline constexpr unsigned int DEFAULT_SEED = 1u;

// default portalID
inline constexpr int PORTAL_ID = -1;