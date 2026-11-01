// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <random>

#include "Maze.hpp"
#include "Metrics.hpp"

/**
 * @brief Bundles a generated maze with its construction metrics.
 */
struct GenerationResult {
  Maze maze;                  ///< The generated maze instance.
  GenerationMetrics metrics;  ///< Graph metrics and execution performance data.
};

/**
 * @brief Factory class providing maze generation algorithms and passage
 * modifications.
 */
class MazeGenerator {
 public:
  /**
   * @brief Generates a maze using randomized Depth-First Search (DFS).
   * @param rows Number of rows in the maze grid.
   * @param cols Number of columns in the maze grid.
   * @param rng Random number generator engine.
   * @return GenerationResult containing the maze and generation metrics.
   */
  static GenerationResult generateDfs(int rows, int cols, std::mt19937& rng);

  /**
   * @brief Generates a maze using randomized Breadth-First Search (BFS).
   * @param rows Number of rows in the maze grid.
   * @param cols Number of columns in the maze grid.
   * @param rng Random number generator engine.
   * @return GenerationResult containing the maze and generation metrics.
   */
  static GenerationResult generateBfs(int rows, int cols, std::mt19937& rng);

  /**
   * @brief Randomly opens closed interior walls to create loops and alternative
   * paths.
   * @param maze Maze instance to modify.
   * @param probability Likelihood of opening each eligible interior wall
   * [0.0, 1.0].
   * @param rng Random number generator engine.
   * @return Number of extra passages added.
   */
  static int addExtraPassages(Maze& maze, double probability,
                              std::mt19937& rng);

  /**
   * @brief Generates a complete maze using the chosen algorithm and adds extra
   * passages.
   * @param rows Number of rows in the maze grid.
   * @param cols Number of columns in the maze grid.
   * @param algorithm Generation algorithm (DFS or BFS).
   * @param extraEdgeProb Probability of opening additional interior walls.
   * @param rng Random number generator engine.
   * @return GenerationResult containing the final maze and combined metrics.
   */
  static GenerationResult generate(int rows, int cols,
                                   GenerationAlgorithm algorithm,
                                   double extraEdgeProb, std::mt19937& rng);
};
