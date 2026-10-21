// Copyright Alejandro, Ashley y Kevin
#pragma once
 
#include <QKeyEvent>
 
/**
 * @brief Keyboard controls of the game. Arrows or WASD move the active player;
 * holding this modifier while pressing a direction uses a Jump Wall instead.
 */
inline constexpr Qt::KeyboardModifier JUMP_WALL_MODIFIER = Qt::ShiftModifier;
