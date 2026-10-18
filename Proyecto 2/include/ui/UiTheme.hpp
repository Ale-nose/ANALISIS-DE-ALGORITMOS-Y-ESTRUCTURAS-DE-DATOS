// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QString>

/**
 * @brief Every color, font and size of the user interface lives here to make
 * it easier to restyle the game. Visible texts live in UiTexts.hpp.
 * Colors are hex strings ("#RRGGBB"), usable both in QSS and in QColor.
 */

// Team palette (5 colors chosen by the group)
inline constexpr const char* PALETTE_FLORAL_WHITE = "#FFF8F0";
inline constexpr const char* PALETTE_JASMINE = "#F4D06F";
inline constexpr const char* PALETTE_ORANGE = "#FF8811";  // Princeton Orange
inline constexpr const char* PALETTE_AQUA = "#9DD9D2";    // Pearl Aqua
inline constexpr const char* PALETTE_INDIGO = "#392F5A";  // Space Indigo

// Shades derived from Space Indigo, for depth (not part of the 5 colors)
inline constexpr const char* PALETTE_INDIGO_DARK = "#2B2347";
inline constexpr const char* PALETTE_INDIGO_LIGHT = "#5B4E8F";

// Base palette (roles)
inline constexpr const char* COLOR_BACKGROUND = PALETTE_INDIGO_DARK;
inline constexpr const char* COLOR_PANEL = PALETTE_INDIGO;
inline constexpr const char* COLOR_BORDER = PALETTE_INDIGO_LIGHT;
inline constexpr const char* COLOR_TEXT = PALETTE_FLORAL_WHITE;
inline constexpr const char* COLOR_TEXT_MUTED = PALETTE_AQUA;

// Accents
inline constexpr const char* COLOR_TITLE = PALETTE_JASMINE;
inline constexpr const char* COLOR_ACCENT = PALETTE_AQUA;
inline constexpr const char* COLOR_BUTTON = PALETTE_ORANGE;
inline constexpr const char* COLOR_BUTTON_HOVER = PALETTE_JASMINE;
inline constexpr const char* COLOR_BUTTON_TEXT = PALETTE_INDIGO;

// Radio buttons and check boxes
inline constexpr const char* COLOR_INDICATOR_BORDER = PALETTE_AQUA;
inline constexpr const char* COLOR_INDICATOR_BG = PALETTE_INDIGO_DARK;

// Game elements
inline constexpr const char* COLOR_PLAYER_1 = PALETTE_AQUA;
inline constexpr const char* COLOR_PLAYER_2 = PALETTE_ORANGE;
inline constexpr const char* COLOR_TREASURE = PALETTE_JASMINE;
inline constexpr const char* COLOR_WALL = PALETTE_FLORAL_WHITE;
// Not in the palette: these need to stay recognizable (mud = brown,
// fog = gray), so they are tinted to blend with the indigo background.
inline constexpr const char* COLOR_MUD = "#B8723A";          // warm brown
inline constexpr const char* COLOR_FOG = "#9B96B0";          // lavender gray
inline constexpr const char* COLOR_PORTAL = "#B79CFF";       // light violet
inline constexpr const char* COLOR_POWER_UP = "#eb759d";     // pink
inline constexpr const char* COLOR_SHIELD_TILE = "#7AA2FF";  // periwinkle

// Typography
inline constexpr const char* UI_FONT_FAMILY = "Segoe UI";
inline constexpr int UI_FONT_SIZE = 15;
inline constexpr int UI_TITLE_FONT_SIZE = 42;
inline constexpr int UI_SUBTITLE_FONT_SIZE = 16;
inline constexpr int UI_BUTTON_FONT_SIZE = 20;

// Shape of borders and group boxes
inline constexpr int UI_BORDER_RADIUS = 12;
inline constexpr int UI_BORDER_WIDTH = 2;
inline constexpr int UI_GROUP_MARGIN_TOP = 16;
inline constexpr int UI_GROUP_PADDING = 14;
inline constexpr int UI_GROUP_TITLE_LEFT = 14;
inline constexpr int UI_GROUP_TITLE_PADDING = 6;

// Options inside a group box (radio buttons and check boxes)
inline constexpr int UI_OPTION_SPACING = 10;
inline constexpr int UI_OPTION_PADDING = 4;
inline constexpr int UI_INDICATOR_SIZE = 18;
inline constexpr int UI_INDICATOR_BORDER_WIDTH = 2;
inline constexpr int UI_CHECKBOX_RADIUS = 4;

// Primary button
inline constexpr int UI_BUTTON_PADDING_VERTICAL = 14;
inline constexpr int UI_BUTTON_PADDING_HORIZONTAL = 28;

// Window layout
inline constexpr int UI_WINDOW_MIN_WIDTH = 460;
inline constexpr int UI_LAYOUT_MARGIN = 32;
inline constexpr int UI_LAYOUT_SPACING = 18;

/**
 * @brief Builds the application-wide stylesheet (QSS) from the constants above.
 * @return A QString containing the fully formatted Qt Style Sheet rules ready
 * to be applied to the main application or widgets.
 */
QString buildStyleSheet();
