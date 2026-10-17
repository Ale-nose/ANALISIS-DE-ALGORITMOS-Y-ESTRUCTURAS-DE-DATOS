// Copyright Alejandro, Ashley y Kevin
#include <iomanip>
#include <iostream>
#include <random>

#include "GameConfig.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

void printGeneration(const char* name, const GenerationResult& result, int rows, int cols) {
  const auto& metrics = result.metrics;

  std::cout << name << "," << rows << "," << cols << "," << metrics.visitedNodes << ",";
  std::cout << metrics.createdEdges << "," << metrics.extraEdges << ",," << metrics.cpuTimeMs << '\n';
}

void printSearch(const char* name, const SearchResult& result, int rows, int cols) {
  const auto& metrics = result.metrics;

  std::cout << name << "," << rows << "," << cols << "," << metrics.visitedNodes << ",,,";
  std::cout << metrics.pathCost << "," << metrics.cpuTimeMs << '\n';
}

int main() {
  std::mt19937 rng(42);

  std::cout << std::fixed << std::setprecision(6);
  std::cout << "algorithm,rows,cols,visitedNodes,createdEdges,extraEdges,pathCost,cpuTimeMs\n";

  for (int size : {10, 20, 40}) {
    int rows = size;
    int cols = size * 2;

    auto dfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, EXTRA_EDGE_PROB, rng);
    auto bfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, EXTRA_EDGE_PROB, rng);

    printGeneration("GenerateDFS", dfs, rows, cols);
    printGeneration("GenerateBFS", bfs, rows, cols);

    Position start{0, 0};
    Position goal{rows - 1, cols - 1};

    auto bfsPath = findPathBfs(dfs.maze, start, goal);
    auto dijkstra = findPathDijkstra(dfs.maze, start, goal);
    auto astar = findPathAStar(dfs.maze, start, goal, false);

    printSearch("SearchBFS", bfsPath, rows, cols);
    printSearch("Dijkstra", dijkstra, rows, cols);
    printSearch("AStar", astar, rows, cols);
  }

  return 0;
}
