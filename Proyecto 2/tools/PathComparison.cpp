// Copyright Alejandro, Ashley y Kevin
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>
#include <random>

#include "Maze.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

/**
 * @brief Represents a single benchmark record comparing pathfinding algorithms.
 */
struct PathComparisonRow {
  int seed;                ///< Random seed used for generation and mud.
  double mudRate;          ///< Probability of mud placement.
  std::string algorithm;   ///< Name of the evaluated algorithm (BFS, Dijkstra, A*).
  int rows;                ///< Total rows in the evaluated maze grid.
  int cols;                ///< Total columns in the evaluated maze grid.
  std::size_t steps;       ///< Number of movement transitions in the found path.
  int visitedNodes;        ///< Total vertices explored during the search.
  int pathCost;            ///< Accumulated movement cost along the path.
  double cpuTimeMs;        ///< Search duration in milliseconds.
};

static std::vector<PathComparisonRow> comparisonRows;

/**
 * @brief Executes BFS, Dijkstra, and A* on a maze and logs comparative metrics.
 */
void comparePathAlgorithms(const Maze& maze, Position start, Position goal, int seed, double mudRate) {
  SearchResult bfsResult = findPathBfs(maze, start, goal);
  SearchResult dijkstraResult = findPathDijkstra(maze, start, goal);
  SearchResult aStarResult = findPathAStar(maze, start, goal);

  if (!bfsResult.found || !dijkstraResult.found || !aStarResult.found) {
    std::cerr << "[WARNING] Path not found for seed " << seed << "\n";
    return;
  }

  std::size_t bfsSteps = bfsResult.path.size() - 1;
  std::size_t dijkstraSteps = dijkstraResult.path.size() - 1;
  std::size_t aStarSteps = aStarResult.path.size() - 1;

  comparisonRows.push_back({seed, mudRate, "BFS", maze.rows(), maze.cols(), bfsSteps,
    bfsResult.metrics.visitedNodes, bfsResult.metrics.pathCost, bfsResult.metrics.cpuTimeMs});

  comparisonRows.push_back({seed, mudRate, "Dijkstra", maze.rows(), maze.cols(), dijkstraSteps,
    dijkstraResult.metrics.visitedNodes, dijkstraResult.metrics.pathCost, dijkstraResult.metrics.cpuTimeMs});

  comparisonRows.push_back({seed, mudRate, "A*", maze.rows(), maze.cols(), aStarSteps,
    aStarResult.metrics.visitedNodes, aStarResult.metrics.pathCost, aStarResult.metrics.cpuTimeMs});
}

/**
 * @brief Exports accumulated path comparison metrics into a CSV file.
 */
bool exportPathComparison(const std::string& filename) {
  std::ofstream file(filename);
  if (!file.is_open()) return false;

  file << "seed,mudRate,algorithm,rows,cols,steps,visitedNodes,pathCost,cpuTimeMs\n";

  for (const auto& row : comparisonRows) {
    file << row.seed << ',' << row.mudRate << ',' << row.algorithm << ','
         << row.rows << ',' << row.cols << ',' << row.steps << ','
         << row.visitedNodes << ',' << row.pathCost << ',' << row.cpuTimeMs << '\n';
  }
  return file.good();
}

/**
 * @brief Generates exactly 27 rows of benchmark data: 3 seeds x 3 mud rates x 3 algorithms.
 */
void runPathBenchmark() {
  const int rows = 20;
  const int cols = 20;
  const std::vector<int> seeds = {42, 100, 314};
  const std::vector<double> mudRates = {0.0, 0.2, 0.5};

  for (int seed : seeds) {
    for (double mudRate : mudRates) {
      std::mt19937 rng(seed);
      // Generation with 0.2 extra edges to ensure multiple paths exist
      GenerationResult gen = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, 0.2, rng);
      Maze maze = gen.maze;

      // Randomly apply mud across the maze based on mudRate
      std::bernoulli_distribution dist(mudRate);
      for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
          if (dist(rng)) {
            maze.cellAt({r, c}).terrain = TerrainType::MUD;
          }
        }
      }

      Position start{0, 0};
      Position goal{rows - 1, cols - 1};
      comparePathAlgorithms(maze, start, goal, seed, mudRate);
    }
  }

  if (!exportPathComparison("data/path_comparison.csv")) {
    throw std::runtime_error("Could not export path comparison CSV");
  }
}

int main() {
  try {
    runPathBenchmark();
    std::cout << "[INFO] Path comparison: 27 rows generated in data/path_comparison.csv\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << '\n';
    return 1;
  }
}
