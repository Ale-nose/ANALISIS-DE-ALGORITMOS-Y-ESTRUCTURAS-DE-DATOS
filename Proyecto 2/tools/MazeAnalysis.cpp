// Copyright Alejandro, Ashley y Kevin
#include <iostream>
#include <random>
#include <stdexcept>
#include <fstream>

#include "Maze.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

inline constexpr int SEED = 42; ///< Default random seed used for benchmark repeatability.

/**
 * @brief Prints structural maze metrics, such as the total dead ends count.
 * @param maze Maze instance to inspect.
 */
void analyzeMazeGeneration(const Maze& maze) {
  int deadEnds = maze.countDeadEnds();
  std::cout << "Dead ends: " << deadEnds << std::endl;
}

/**
 * @brief Compares DFS and BFS maze generation characteristics and writes metrics to a CSV file.
 *
 * Evaluates dead ends, generation metrics, and Dijkstra path costs between opposite corners
 * for both algorithms using identical dimensions and edge probabilities.
 *
 * @param rows Number of rows in the test maze.
 * @param cols Number of columns in the test maze.
 * @param extraEdgeProb Probability of opening additional interior walls [0.0, 1.0].
 * @throws std::runtime_error If pathfinding fails or if the output file cannot be accessed.
 */
void compareGenerationAlgorithms(int rows, int cols, double extraEdgeProb) {
  std::mt19937 dfsRng(SEED);
  std::mt19937 bfsRng(SEED);

  GenerationResult dfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, extraEdgeProb, dfsRng);
  GenerationResult bfsResult = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, extraEdgeProb, bfsRng);

  std::cout << "DFS:" << std::endl;
  analyzeMazeGeneration(dfsResult.maze);

  std::cout << "BFS:" << std::endl;
  analyzeMazeGeneration(bfsResult.maze);

  Position start{0, 0};
  Position goal{rows - 1, cols - 1};

  SearchResult dfsPath = findPathDijkstra(dfsResult.maze, start, goal);
  SearchResult bfsPath = findPathDijkstra(bfsResult.maze, start, goal);

  if (!dfsPath.found || !bfsPath.found) {
    throw std::runtime_error("Error: path not found");
  }

  std::cout << "DFS path cost: " << dfsPath.metrics.pathCost << std::endl;
  std::cout << "BFS path cost: " << bfsPath.metrics.pathCost << std::endl;
  std::cout << std::endl;

  std::cout << "DFS generation metrics:" << std::endl;
  std::cout << "  Visited nodes: " << dfsResult.metrics.visitedNodes << std::endl;
  std::cout << "  Created edges: " << dfsResult.metrics.createdEdges << std::endl;
  std::cout << "  Extra edges: " << dfsResult.metrics.extraEdges << std::endl;
  std::cout << "  CPU time: " << dfsResult.metrics.cpuTimeMs << std::endl;
  std::cout << std::endl;

  std::cout << "BFS generation metrics:" << std::endl;
  std::cout << "  Visited nodes: " << bfsResult.metrics.visitedNodes << std::endl;
  std::cout << "  Created edges: " << bfsResult.metrics.createdEdges << std::endl;
  std::cout << "  Extra edges: " << bfsResult.metrics.extraEdges << std::endl;
  std::cout << "  CPU time: " << bfsResult.metrics.cpuTimeMs << std::endl;
  std::cout << std::endl;

  
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
    file << SEED << ',' << algorithm << ',' << rows << ',' << cols << ',' << extraEdgeProb << ',';
    file << result.maze.countDeadEnds() << ',' << path.metrics.pathCost << ',';
    file << result.metrics.visitedNodes << ',' << result.metrics.createdEdges << ',';
    file << result.metrics.extraEdges << ',' << result.metrics.cpuTimeMs << '\n';
  };

  writeRow("DFS", dfsResult, dfsPath);
  writeRow("BFS", bfsResult, bfsPath);

  file.close();

  if (!file) {
    throw std::runtime_error("Could not write CSV file");
  }
}
