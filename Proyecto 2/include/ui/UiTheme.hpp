// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QString>

/**
 * @brief Every color, font and size of the user interface lives here to make
 * it easier to restyle the game. Visible texts live in UiTexts.hpp.
 * Colors are hex strings ("#RRGGBB"), usable both in QSS and in QColor.
 */

// Base palette (dark teal, inspired by the reference maze game)
inline constexpr const char* COLOR_BACKGROUND = "#07161B";  // near-black teal
inline constexpr const char* COLOR_PANEL = "#0E2B31";       // dark teal panel
inline constexpr const char* COLOR_BORDER = "#1F6F78";      // muted teal border
inline constexpr const char* COLOR_TEXT = "#E8F6F6";        // almost white
inline constexpr const char* COLOR_TEXT_MUTED = "#7FA9AD";  // grayish teal

// Accents
inline constexpr const char* COLOR_TITLE = "#5CE1E6";         // electric cyan
inline constexpr const char* COLOR_ACCENT = "#5CE1E6";        // electric cyan
inline constexpr const char* COLOR_BUTTON = "#3FBF6B";        // green
inline constexpr const char* COLOR_BUTTON_HOVER = "#5ED688";  // light green
inline constexpr const char* COLOR_BUTTON_TEXT = "#07161B";   // dark on green

// Radio buttons and check boxes
inline constexpr const char* COLOR_INDICATOR_BORDER = "#7FA9AD";  // visible edge
inline constexpr const char* COLOR_INDICATOR_BG = "#07161B";      // empty fill

// Game elements
inline constexpr const char* COLOR_PLAYER_1 = "#5CE1E6";     // cyan
inline constexpr const char* COLOR_PLAYER_2 = "#FF5C8A";     // pink
inline constexpr const char* COLOR_TREASURE = "#FFC857";     // soft gold
inline constexpr const char* COLOR_MUD = "#9C6B3F";          // brown
inline constexpr const char* COLOR_FOG = "#8A9BA0";          // gray
inline constexpr const char* COLOR_PORTAL = "#A56BFF";       // purple
inline constexpr const char* COLOR_POWER_UP = "#9BE564";     // lime green
inline constexpr const char* COLOR_SHIELD_TILE = "#6C9EFF";  // soft blue
inline constexpr const char* COLOR_WALL = "#1E6F77";         // teal block

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