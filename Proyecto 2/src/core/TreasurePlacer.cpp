// Copyright Alejandro, Ashley y Kevin
#include <stdexcept>
#include <cmath>

#include "TreasurePlacer.hpp"

Position chooseTreasurePosition(const Maze& maze, const SearchResult& shortestPath) {
  // Ensure a valid, non-empty path was provided
  if (shortestPath.found == false || shortestPath.path.empty()) {
    throw std::invalid_argument("Invalid argument");
  }

  // Target cost value corresponding to the exact midpoint of the route
  int totalCost = shortestPath.metrics.pathCost;
  double halfCost = totalCost / 2.0;

  // Track the candidate cell closest to half of the total path cost
  Position bestPosition = shortestPath.path[0];
  int accumulatedCost = 0;
  double bestDifference = halfCost;

  // Walk through the path from start to goal accumulating entry costs
  for (std::size_t i = 1; i < shortestPath.path.size(); i++) {
    Position current = shortestPath.path[i];
    accumulatedCost += maze.entryCost(current);
    double difference = std::abs(accumulatedCost - halfCost);

    // Update the best candidate when finding a smaller difference to halfCost
    if (difference < bestDifference) {
      bestDifference = difference;
      bestPosition = current;
    }
  }

  return bestPosition;
}
