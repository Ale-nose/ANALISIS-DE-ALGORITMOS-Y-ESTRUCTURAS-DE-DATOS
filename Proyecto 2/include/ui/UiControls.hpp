// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QKeyEvent>

/**
 * @brief Keyboard controls of the game. Arrows or WASD move the active player;
 * holding this modifier while pressing a direction uses a Jump Wall instead.
 */
inline constexpr Qt::KeyboardModifier JUMP_WALL_MODIFIER = Qt::ShiftModifier;

// AI turns: pause before each AI action so the human can follow the match
inline constexpr int AI_ACTION_DELAY_MS = 600;

// Time to see the move
inline constexpr int HANDOFF_PREVIEW_DELAY_MS = 900;

// Extra: animated A* search visualizer
constexpr int SEARCH_STEP_INTERVAL_MS = 40;

// Extra: Network
inline constexpr const char* ONLINE_DEFAULT_ADDRESS = "127.0.0.1";
inline constexpr int ONLINE_DEFAULT_PORT = 5000;
inline constexpr int ONLINE_PORT_MIN = 1024;
inline constexpr int ONLINE_PORT_MAX = 65535;
inline constexpr int ONLINE_MESSAGE_MS =
    4000;  // how long a notice stays in the status bar
