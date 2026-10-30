// Copyright Alejandro, Ashley y Kevin
#include <iostream>
#include <random>
#include <stdexcept>
#include <fstream>
#include <vector>

#include "Maze.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

/**
 * @brief Compares DFS and BFS maze generation characteristics and writes metrics to CSV.
 *
 * Both generators receive the same seed, so the two rows of a combination
 * describe the same experiment run with a different algorithm.
 *
 * @param file Open CSV file the rows are appended to.
 * @param rows Rows of the generated maze.
 * @param cols Columns of the generated maze.
 * @param extraEdgeProb Probability of opening each remaining internal wall.
 * @param seed Seed shared by both generators.
 */
void compareGenerationAlgorithms(std::ofstream& file, int rows, int cols, double extraEdgeProb, int seed) {
  std::mt19937 dfsRng(seed);
  std::mt19937 bfsRng(seed);

  GenerationResult dfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, extraEdgeProb, dfsRng);
  GenerationResult bfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, extraEdgeProb, bfsRng);

  Position start{0, 0};
  Position goal{rows - 1, cols - 1};

  SearchResult dfsPath = findPathDijkstra(dfsResult.maze, start, goal);
  SearchResult bfsPath = findPathDijkstra(bfsResult.maze, start, goal);

  if (!dfsPath.found || !bfsPath.found) {
    std::cerr << "[WARNING] Path not found for seed " << seed;
    std::cerr << " on " << rows << "x" << cols << "\n";
    return;
  }

  auto writeRow = [&](const char* algorithm, const GenerationResult& result, const SearchResult& path) {
    file << seed << ',' << algorithm << ',' << rows << ',' << cols << ',' << extraEdgeProb << ',';
    file << result.maze.countDeadEnds() << ',' << path.metrics.pathCost << ',';
    file << result.metrics.visitedNodes << ',' << result.metrics.createdEdges << ',';
    file << result.metrics.extraEdges << ',' << result.metrics.cpuTimeMs << '\n';
  };

  writeRow("DFS", dfsResult, dfsPath);
  writeRow("BFS", bfsResult, bfsPath);
}

/**
 * @brief Generates the maze comparison data set.
 *
 * Four square sizes, five seeds and three extra edge probabilities give
 * 120 rows. The sizes let the report plot how the generation time and the
 * explored nodes grow with the number of cells.
 *
 * @return Exit status 0 when the CSV was written.
 */
int main() {
  const std::vector<int> sizes = {10, 20, 30, 40};
  const std::vector<int> seeds = {42, 100, 314, 7, 2026};
  const std::vector<double> extraProbs = {0.0, 0.2, 0.5};

  // The file is truncated on every run so the data set is always complete
  std::ofstream file("data/maze_comparison.csv", std::ios::trunc);
  if (!file.is_open()) {
    std::cerr << "[ERROR] Could not open data/maze_comparison.csv\n";
    return 1;
  }

  file << "seed,algorithm,rows,cols,extraEdgeProb,deadEnds,pathCost,";
  file << "visitedNodes,createdEdges,extraEdges,cpuTimeMs\n";

  try {
    for (int size : sizes) {
      for (int seed : seeds) {
        for (double prob : extraProbs) {
          compareGenerationAlgorithms(file, size, size, prob, seed);
        }
      }
    }
    std::cout << "[INFO] Maze comparison: ";
    std::cout << sizes.size() * seeds.size() * extraProbs.size() * 2;
    std::cout << " rows generated in data/maze_comparison.csv\n";
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << '\n';
    return 1;
  }
  return 0;
}