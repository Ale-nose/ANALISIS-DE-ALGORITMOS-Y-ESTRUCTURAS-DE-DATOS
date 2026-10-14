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
};
