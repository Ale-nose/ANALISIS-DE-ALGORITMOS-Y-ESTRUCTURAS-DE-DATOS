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
