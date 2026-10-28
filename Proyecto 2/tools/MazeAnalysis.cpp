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
 */
void compareGenerationAlgorithms(int rows, int cols, double extraEdgeProb, int seed) {
  std::mt19937 dfsRng(seed);
  std::mt19937 bfsRng(seed);

  GenerationResult dfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, extraEdgeProb, dfsRng);
  GenerationResult bfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, extraEdgeProb, bfsRng);

  Position start{0, 0};
  Position goal{rows - 1, cols - 1};

  SearchResult dfsPath = findPathDijkstra(dfsResult.maze, start, goal);
  SearchResult bfsPath = findPathDijkstra(bfsResult.maze, start, goal);

  if (!dfsPath.found || !bfsPath.found) {
    std::cerr << "[WARNING] Path not found for seed " << seed << "\n";
    return;
  }

  const std::string filename = "data/maze_comparison.csv";

  std::ifstream previousFile(filename, std::ios::binary | std::ios::ate);
  bool writeHeader = !previousFile.is_open() || previousFile.tellg() == std::streampos(0);
  previousFile.close();

  std::ofstream file(filename, std::ios::app);
  if (!file.is_open()) {
    throw std::runtime_error("Could not open CSV file");
  }

  if (writeHeader) {
    file << "seed,algorithm,rows,cols,extraEdgeProb,deadEnds,pathCost,";
    file << "visitedNodes,createdEdges,extraEdges,cpuTimeMs\n";
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

int main() {
  const int rows = 20;
  const int cols = 20;
  const std::vector<int> seeds = {42, 100, 314};
  const std::vector<double> extraProbs = {0.0, 0.2, 0.5};

  // Clear the file on every run to guarantee exactly 18 rows
  std::ofstream clearFile("data/maze_comparison.csv", std::ios::trunc);
  clearFile.close();

  try {
    for (int seed : seeds) {
      for (double prob : extraProbs) {
        compareGenerationAlgorithms(rows, cols, prob, seed);
      }
    }
    std::cout << "[INFO] Maze comparison: 18 rows generated in data/maze_comparison.csv\n";
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << '\n';
    return 1;
  }
  return 0;
}
