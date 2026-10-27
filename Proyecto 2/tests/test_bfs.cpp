// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>
#include <random>

#include "MazeGenerator.hpp"

void testBfsGeneration() {
  // Seeded pseudorandom generator for deterministic maze creation
  std::mt19937 rng(42);

  // Generate a 3x4 maze using BFS spanning tree algorithm
  GenerationResult result = MazeGenerator::generateBfs(3, 4, rng);

  // Output generated metrics and dimensions to console
  std::cout << "Visited nodes: " << result.metrics.visitedNodes << std::endl;
  std::cout << "Created edges: " << result.metrics.createdEdges << std::endl;
  std::cout << "Rows: " << result.maze.rows() << std::endl;
  std::cout << "Cols: " << result.maze.cols() << std::endl;

  // Test boundary validation for adjacent positions
  std::cout << "Inside (0,1): " << result.maze.isInside({0, 1}) << std::endl;
  std::cout << "Inside (1,0): " << result.maze.isInside({1, 0}) << std::endl;

  // Spanning tree assertions: all 12 nodes visited, V-1 = 11 edges created, no loops
  assert(result.metrics.visitedNodes == 12);
  assert(result.metrics.createdEdges == 11);
  assert(result.metrics.extraEdges == 0);

  // Dimension assertions
  assert(result.maze.rows() == 3);
  assert(result.maze.cols() == 4);

  std::cout << "All BFS generation tests passed!" << std::endl;
}
