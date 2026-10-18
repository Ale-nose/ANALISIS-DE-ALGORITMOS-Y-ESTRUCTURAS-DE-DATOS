// Copyright Alejandro, Ashley y Kevin
#pragma once

#include "UiTheme.hpp"

/**
 * @brief Colors and sizes used only by the game board. Element colors live in 
 * UiTheme.hpp. This file adds what the board needs on top: its own background,
 * the floor of the cells, and every proportion used to draw the icons.
 */

// Board colors
inline constexpr const char* COLOR_BOARD_BACKGROUND = "#2B2347";  // dark indigo
inline constexpr const char* COLOR_CELL_FLOOR = PALETTE_INDIGO;
inline constexpr const char* COLOR_ICON_OUTLINE = COLOR_BOARD_BACKGROUND;
inline constexpr const char* COLOR_ICON_TEXT = PALETTE_INDIGO;      // on bright icons
inline constexpr const char* COLOR_ACTIVE_PLAYER_RING = PALETTE_FLORAL_WHITE;

// Board size
inline constexpr int BOARD_MARGIN = 16;
inline constexpr int BOARD_PREFERRED_CELL_SIZE = 40;
inline constexpr int BOARD_MIN_CELL_SIZE = 20;

// Walls and outlines
inline constexpr double BOARD_WALL_RATIO = 0.08;
inline constexpr double BOARD_MIN_WALL_WIDTH = 2.0;
inline constexpr double BOARD_OUTLINE_RATIO = 0.03;
inline constexpr double BOARD_MIN_OUTLINE_WIDTH = 1.0;

// Icons
inline constexpr double BOARD_PLAYER_RATIO = 0.60;
inline constexpr double BOARD_ACTIVE_RING_RATIO = 0.80;
inline constexpr double BOARD_ACTIVE_RING_WIDTH_RATIO = 0.05;
inline constexpr double BOARD_TREASURE_RATIO = 0.75;
inline constexpr double BOARD_POWER_UP_RATIO = 0.52;
inline constexpr double BOARD_BADGE_CORNER_RATIO = 0.25;  // of the badge side
inline constexpr double BOARD_SHIELD_RATIO = 0.62;
inline constexpr double BOARD_PORTAL_RATIO = 0.74;
inline constexpr double BOARD_PORTAL_INNER_RATIO = 0.55;  // of the outer ring
inline constexpr double BOARD_PORTAL_RING_RATIO = 0.06;
inline constexpr double BOARD_PORTAL_LABEL_RATIO = 0.24;  // font size

// Letters and numbers drawn inside icons
inline constexpr double BOARD_LABEL_RATIO = 0.32;  // font size
