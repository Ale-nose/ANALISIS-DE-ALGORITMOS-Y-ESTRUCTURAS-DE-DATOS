// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <vector>

#include "GameTypes.hpp"
#include "Metrics.hpp"
#include "Maze.hpp"

// Search visualization data
struct SearchTrace {
  std::vector<Position> opened;
  std::vector<Position> closed;
  std::vector<Position> path;
};

// Pathfinding result
struct SearchResult {
  bool found = false;
  std::vector<Position> path;
  SearchMetrics metrics;
  SearchTrace trace;
};

struct SearchResult findPathDijkstra(const Maze& maze, Position start, Position goal);
