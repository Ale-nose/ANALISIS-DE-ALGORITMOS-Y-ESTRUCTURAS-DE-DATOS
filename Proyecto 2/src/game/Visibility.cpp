// Copyright Alejandro, Ashley y Kevin


#include "Pathfinding.hpp"
#include "Visibility.hpp"

bool isCellVisible(Position viewer, Position target, int radius) {

  return manhattanDistance(viewer,target) <= radius;

}