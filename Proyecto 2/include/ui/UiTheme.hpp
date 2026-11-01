// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QString>

/**
 * @brief Core color palette of the game. Other theme files (BoardTheme,
 * HUDTheme, MenuTheme) include this file to use these base colors.
 */

// Team palette
inline constexpr const char* PALETTE_FLORAL_WHITE = "#FFF8F0";
inline constexpr const char* PALETTE_JASMINE = "#F4D06F";
inline constexpr const char* PALETTE_ORANGE = "#FF8811";  // Princeton Orange
inline constexpr const char* PALETTE_AQUA = "#9DD9D2";    // Pearl Aqua
inline constexpr const char* PALETTE_INDIGO = "#392F5A";  // Space Indigo

// Shades derived from Space Indigo, for depth
inline constexpr const char* PALETTE_INDIGO_DARK = "#2B2347";
inline constexpr const char* PALETTE_INDIGO_LIGHT = "#5B4E8F";

// Game elements
inline constexpr const char* COLOR_PLAYER_1 = PALETTE_AQUA;
inline constexpr const char* COLOR_PLAYER_2 = PALETTE_ORANGE;
inline constexpr const char* COLOR_TREASURE = PALETTE_JASMINE;
inline constexpr const char* COLOR_WALL = PALETTE_FLORAL_WHITE;

// Not in the palette: these need to stay recognizable, so they are tinted to
// blend with the indigo background.
inline constexpr const char* COLOR_MUD = "#B8723A";          // warm brown
inline constexpr const char* COLOR_FOG = "#9B96B0";          // lavender gray
inline constexpr const char* COLOR_PORTAL = "#B79CFF";       // light violet
inline constexpr const char* COLOR_POWER_UP = "#eb759d";     // pink
inline constexpr const char* COLOR_SHIELD_TILE = "#7AA2FF";  // periwinkle
