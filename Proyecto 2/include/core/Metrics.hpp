// Copyright Alejandro, Ashley y Kevin
#pragma once

// Available pathfinding algorithms
enum class SearchAlgorithm { BFS, DIJKSTRA, A_STAR };

// Maze generation statistics
struct GenerationMetrics {
  int visitedNodes = 0;
  int createdEdges = 0;
  int extraEdges = 0;
  double cpuTimeMs = 0.0;
};

// Pathfinding statistics
struct SearchMetrics {
  int visitedNodes = 0;
  int pathCost = 0;
  double cpuTimeMs = 0.0;
};
