// Copyright Alejandro, Ashley y Kevin

#include "Visibility.hpp"

#include "Pathfinding.hpp"

bool isCellVisible(Position viewer, Position target, int radius) {
  return manhattanDistance(viewer, target) <= radius;
}
