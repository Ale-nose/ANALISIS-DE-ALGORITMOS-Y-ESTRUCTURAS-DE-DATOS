// Copyright Alejandro, Ashley y Kevin
#include <stdexcept>
#include <cmath>

#include "TreasurePlacer.hpp"

Position chooseTreasurePosition(const Maze& maze, const SearchResult& shortestPath) {
  if (shortestPath.found == false || shortestPath.path.empty()) {
    throw std::invalid_argument("Invalid argument");
  }

  int totalCost = shortestPath.metrics.pathCost;
  double halfCost = totalCost / 2.0;

  Position bestPosition = shortestPath.path[0];
  int accumulatedCost = 0;
  double bestDifference = halfCost;

  for (std::size_t i = 1; i < shortestPath.path.size(); i++) {
    Position current = shortestPath.path[i];
    accumulatedCost += maze.entryCost(current);
    double difference = std::abs(accumulatedCost - halfCost);

    if (difference < bestDifference) {
      bestDifference = difference;
      bestPosition = current;
    }
  }

  return bestPosition;
}
