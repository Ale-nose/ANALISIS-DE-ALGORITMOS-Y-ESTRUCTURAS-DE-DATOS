// Copyright Alejandro, Ashley y Kevin
#pragma once

#include "UiTheme.hpp"

/**
 * @brief Colors and sizes used only by the game board. Element images live in
 * UiAssets.hpp and the colors shared with the rest of the interface live in
 * UiTheme.hpp. This file adds what the board needs on top: its own background,
 * the floor of the cells and the proportions used to place the images.
 */

// Board colors
inline constexpr const char* COLOR_BOARD_BACKGROUND = PALETTE_INDIGO_DARK;
inline constexpr const char* COLOR_CELL_FLOOR = PALETTE_INDIGO;
inline constexpr const char* COLOR_ACTIVE_PLAYER_RING = PALETTE_FLORAL_WHITE;
inline constexpr const char* COLOR_PORTAL_LABEL = PALETTE_FLORAL_WHITE;
inline constexpr const char* COLOR_PORTAL_BADGE = PALETTE_INDIGO_DARK;

// Board size (the images need room to stay readable)
inline constexpr int BOARD_MARGIN = 16;
inline constexpr int BOARD_PREFERRED_CELL_SIZE = 48;
inline constexpr int BOARD_MIN_CELL_SIZE = 32;

// Walls and ring
inline constexpr double BOARD_WALL_RATIO = 0.08;
inline constexpr double BOARD_MIN_WALL_WIDTH = 2.0;
inline constexpr double BOARD_MIN_OUTLINE_WIDTH = 1.0;

// Images: side of each image as a fraction of the cell side
inline constexpr double BOARD_PLAYER_RATIO = 0.80;
inline constexpr double BOARD_ACTIVE_RING_RATIO = 0.95;
inline constexpr double BOARD_ACTIVE_RING_WIDTH_RATIO = 0.05;
inline constexpr double BOARD_TREASURE_RATIO = 0.80;
inline constexpr double BOARD_POWER_UP_RATIO = 0.75;
inline constexpr double BOARD_SHIELD_RATIO = 0.75;
inline constexpr double BOARD_PORTAL_RATIO = 0.90;
inline constexpr double BOARD_MUD_RATIO = 0.95;
inline constexpr double BOARD_PORTAL_BADGE_RATIO = 0.46;
inline constexpr double BOARD_PORTAL_LABEL_RATIO = 0.28;

// Screen space the window cannot use (title bar and window frame)
inline constexpr int BOARD_SCREEN_RESERVED_WIDTH = 40;
inline constexpr int BOARD_SCREEN_RESERVED_HEIGHT = 160;
