// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <random>

#include "Metrics.hpp"
#include "Maze.hpp"

struct GenerationResult {
  Maze maze;
  GenerationMetrics metrics;
};

class MazeGenerator {
 public:
  static GenerationResult generateDfs(int rows, int cols, std::mt19937& rng);

  static GenerationResult generateBfs(int rows, int cols, std::mt19937& rng);

  static int addExtraPassages(Maze& maze, double probability, std::mt19937& rng);

  static GenerationResult generate(
    int rows, int cols, GenerationAlgorithm algorithm, double extraEdgeProb, std::mt19937& rng);
};
