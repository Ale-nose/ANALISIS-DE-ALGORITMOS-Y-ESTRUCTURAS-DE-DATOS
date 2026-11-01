// Copyright Alejandro, Ashley y Kevin

#pragma once

#include "GameConfig.hpp"
#include "GameTypes.hpp"

/**
 * @brief Tells whether a cell is inside the field of view of a player.
 *
 * A cell is visible if its Manhattan distance to the viewer is less than or
 * equal to the radius. Walls are not taken into account.
 *
 * @param viewer Cell where the player stands.
 * @param target Cell to check.
 * @param radius Maximum distance at which a cell is visible. FOG_RADIUS by
 *               default.
 * @return true if the cell is visible, false otherwise.
 */
bool isCellVisible(Position viewer, Position target, int radius = FOG_RADIUS);
