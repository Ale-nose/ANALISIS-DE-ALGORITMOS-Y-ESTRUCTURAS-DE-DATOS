// Copyright Alejandro, Ashley y Kevin
#pragma once

/**
 * @brief Contains all the visible strings and text labels used in the game's User Interface.
 * Centralizing texts here makes it easier to modify them or translate the game in the future.
 */

// Start window texts
inline constexpr const char* TEXT_WINDOW_TITLE = "Maze Craze";
inline constexpr const char* TEXT_TITLE = "MAZE CRAZE";
inline constexpr const char* TEXT_SUBTITLE = "Neural Pathfinding & Grid Tactics";
inline constexpr const char* TEXT_GROUP_MODE = "Game mode";
inline constexpr const char* TEXT_MODE_SINGLE = "Single Player (vs AI)";
inline constexpr const char* TEXT_MODE_LOCAL = "Two Players (same computer)";
inline constexpr const char* TEXT_GROUP_GENERATOR = "Maze generation";
inline constexpr const char* TEXT_GENERATOR_DFS = "Deep-Sector Probe (long, winding paths)";
inline constexpr const char* TEXT_GENERATOR_BFS = "Broad-Wave Scan (shorter paths)";
inline constexpr const char* TEXT_FOG = "Enable Fog of War";
inline constexpr const char* TEXT_START = "START";

// Board texts: short letters drawn on the power-up badges and Shield Tiles
inline constexpr const char* TEXT_BOARD_JUMP_WALL = "J";
inline constexpr const char* TEXT_BOARD_DOUBLE_PLAY = "D";
inline constexpr const char* TEXT_BOARD_CONTROL_ENEMY = "C";
inline constexpr const char* TEXT_BOARD_SHIELD_TILE = "S";

// Error texts (%1 = rows and %2 = columns)
inline constexpr const char* TEXT_ERROR_BOARD_DOES_NOT_FIT = 
  "The %1 x %2 maze does not fit on this screen. Choose smaller dimensions and "
  "try again.";
inline constexpr const char* TEXT_ERROR_MAP_GENERATION =
  "The maze could not be generated. Please try again.";

// HUD
inline constexpr const char* TEXT_HUD_PLAYER_1 = "Player 1";
inline constexpr const char* TEXT_HUD_PLAYER_2 = "Player 2";
inline constexpr const char* TEXT_HUD_AI = "AI";
inline constexpr const char* TEXT_HUD_TURN = "Turn: %1";
inline constexpr const char* TEXT_HUD_ACTIONS = "Actions left: %1";
inline constexpr const char* TEXT_HUD_JUMP_WALLS = "Jump Walls: %1";
inline constexpr const char* TEXT_HUD_SHIELDS = "Shields: %1";
inline constexpr const char* TEXT_HUD_MUD = "Stuck in mud: %1 pending actions";
inline constexpr const char* TEXT_HUD_OPPONENT_SKIPS = 
  "The opponent will lose their next turn";
inline constexpr const char* TEXT_HUD_HIDDEN = "?";
