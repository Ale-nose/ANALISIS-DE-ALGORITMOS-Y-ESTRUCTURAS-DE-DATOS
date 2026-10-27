// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QString>

#include "UiTheme.hpp"

/**
 * @brief Colors, fonts, and layout sizes specific to the Start Window (Main Menu).
 * It builds the QSS stylesheet for the UI components.
 */

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
