// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>
#include <random>

#include "MazeGenerator.hpp"

int main() {
  std::mt19937 rng(42);

  // Test with 0% probability
  auto result1 = MazeGenerator::generate(3, 4, GenerationAlgorithm::DFS, 0.0, rng);

  assert(result1.metrics.visitedNodes == 12);
  assert(result1.metrics.createdEdges == 11);
  assert(result1.metrics.extraEdges == 0);

  // Test with 100% probability
  auto result2 = MazeGenerator::generate(3, 4, GenerationAlgorithm::BFS, 1.0, rng);

  assert(result2.metrics.visitedNodes == 12);
  assert(result2.metrics.createdEdges == 17);
  assert(result2.metrics.extraEdges == 6);

  std::cout << "All extra passages tests passed!\n";

  return 0;
}
