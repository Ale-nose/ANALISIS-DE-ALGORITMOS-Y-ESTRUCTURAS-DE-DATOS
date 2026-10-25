// Copyright Alejandro, Ashley y Kevin
#include <iomanip>
#include <iostream>
#include <random>

#include "GameConfig.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

/**
 * @brief Outputs maze generation metrics in CSV format.
 * @param name Label identifying the generation algorithm used.
 * @param result Generation result containing the generated maze and execution metrics.
 * @param rows Total number of rows in the tested grid.
 * @param cols Total number of columns in the tested grid.
 */
void printGeneration(const char* name, const GenerationResult& result, int rows, int cols) {
  const auto& metrics = result.metrics;

  std::cout << name << "," << rows << "," << cols << "," << metrics.visitedNodes << ",";
  std::cout << metrics.createdEdges << "," << metrics.extraEdges << ",," << metrics.cpuTimeMs << '\n';
}

/**
 * @brief Outputs pathfinding search metrics in CSV format.
 * @param name Label identifying the pathfinding algorithm evaluated.
 * @param result Search result containing path status, cost, and execution metrics.
 * @param rows Total number of rows in the evaluated maze grid.
 * @param cols Total number of columns in the evaluated maze grid.
 */
void printSearch(const char* name, const SearchResult& result, int rows, int cols) {
  const auto& metrics = result.metrics;

  std::cout << name << "," << rows << "," << cols << "," << metrics.visitedNodes << ",,,";
  std::cout << metrics.pathCost << "," << metrics.cpuTimeMs << '\n';
}

/**
 * @brief Benchmark executable comparing performance and metrics of maze generation and search algorithms.
 * @return Exit status 0 on successful benchmark execution.
 */
int main() {
  std::mt19937 rng(42);

  std::cout << std::fixed << std::setprecision(6);
  std::cout << "algorithm,rows,cols,visitedNodes,createdEdges,extraEdges,pathCost,cpuTimeMs\n";

  for (int size : {10, 20, 40}) {
    int rows = size;
    int cols = size * 2;

    GenerationResult dfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, EXTRA_EDGE_PROB, rng);
    GenerationResult bfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, EXTRA_EDGE_PROB, rng);

    printGeneration("GenerateDFS", dfs, rows, cols);
    printGeneration("GenerateBFS", bfs, rows, cols);

    Position start{0, 0};
    Position goal{rows - 1, cols - 1};

    SearchResult bfsPath = findPathBfs(dfs.maze, start, goal);
    SearchResult dijkstra = findPathDijkstra(dfs.maze, start, goal);
    SearchResult astar = findPathAStar(dfs.maze, start, goal, false);

    printSearch("SearchBFS", bfsPath, rows, cols);
    printSearch("Dijkstra", dijkstra, rows, cols);
    printSearch("AStar", astar, rows, cols);
  }

  return 0;
}
