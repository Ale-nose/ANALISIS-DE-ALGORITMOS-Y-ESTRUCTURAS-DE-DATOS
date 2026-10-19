// Copyright Alejandro, Ashley y Kevin
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>

#include "Maze.hpp"
#include "Pathfinding.hpp"

struct PathComparisonRow {
  std::string algorithm;
  int rows;
  int cols;
  std::size_t steps;
  int visitedNodes;
  int pathCost;
  double cpuTimeMs;
};

static std::vector<PathComparisonRow> comparisonRows;

void comparePathAlgorithms(const Maze& maze, Position start, Position goal) {
  SearchResult bfsResult = findPathBfs(maze, start, goal);
  SearchResult dijkstraResult = findPathDijkstra(maze, start, goal);
  SearchResult aStarResult = findPathAStar(maze, start, goal);

  if (!bfsResult.found || !dijkstraResult.found || !aStarResult.found) {
    throw std::runtime_error("Path not found");
  }

  std::size_t bfsSteps = bfsResult.path.size() - 1;
  std::size_t dijkstraSteps = dijkstraResult.path.size() - 1;
  std::size_t aStarSteps = aStarResult.path.size() - 1;

  if (dijkstraResult.metrics.pathCost != aStarResult.metrics.pathCost) {
    throw std::runtime_error("Dijkstra and A* cost differ");
  }

  std::cout << "BFS:" << std::endl;
  std::cout << "Steps: " << bfsSteps << std::endl;
  std::cout << "Visited nodes: " << bfsResult.metrics.visitedNodes << std::endl;
  std::cout << "Path cost: " << bfsResult.metrics.pathCost << std::endl;
  std::cout << "CPU time: " << bfsResult.metrics.cpuTimeMs << std::endl;
  std::cout << std::endl;

  std::cout << "Dijkstra:" << std::endl;
  std::cout << "Steps: " << dijkstraSteps << std::endl;
  std::cout << "Visited nodes: " << dijkstraResult.metrics.visitedNodes << std::endl;
  std::cout << "Path cost: " << dijkstraResult.metrics.pathCost << std::endl;
  std::cout << "CPU time: " << dijkstraResult.metrics.cpuTimeMs << std::endl;
  std::cout << std::endl;

  std::cout << "A*:" << std::endl;
  std::cout << "Steps: " << aStarSteps << std::endl;
  std::cout << "Visited nodes: " << aStarResult.metrics.visitedNodes << std::endl;
  std::cout << "Path cost: " << aStarResult.metrics.pathCost << std::endl;
  std::cout << "CPU time: " << aStarResult.metrics.cpuTimeMs << std::endl;
  std::cout << std::endl;

  PathComparisonRow bfsRow{"BFS", maze.rows(), maze.cols(), bfsSteps, bfsResult.metrics.visitedNodes,
    bfsResult.metrics.pathCost, bfsResult.metrics.cpuTimeMs};

  comparisonRows.push_back(bfsRow);

  PathComparisonRow dijkstraRow{"Dijkstra", maze.rows(), maze.cols(), dijkstraSteps,
    dijkstraResult.metrics.visitedNodes, dijkstraResult.metrics.pathCost, dijkstraResult.metrics.cpuTimeMs};

  comparisonRows.push_back(dijkstraRow);

  PathComparisonRow aStarRow{"A*", maze.rows(), maze.cols(), aStarSteps, aStarResult.metrics.visitedNodes,
    aStarResult.metrics.pathCost, aStarResult.metrics.cpuTimeMs};

  comparisonRows.push_back(aStarRow);
}

bool exportPathComparison(const std::string& filename) {
  std::ofstream file(filename);

  if (!file.is_open()) {
    return false;
  }

  file << "algorithm,rows,cols,steps,visitedNodes,pathCost,cpuTimeMs\n";

  for (const auto& row : comparisonRows) {
    file << row.algorithm << ',' << row.rows << ',' << row.cols << ',';
    file << row.steps << ',' << row.visitedNodes << ',' << row.pathCost << ',';
    file << row.cpuTimeMs << '\n';
  }

  file.close();
  return file.good();
}

void compareMudScenario() {
  Maze maze(2, 5);

  for (int col = 0; col <= 3; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
    maze.openPassage({1, col}, Direction::RIGHT);
  }

  maze.openPassage({0, 0}, Direction::DOWN);
  maze.openPassage({0, 4}, Direction::DOWN);

  for (int col = 1; col <= 3; col++) {
    maze.cellAt({0, col}).terrain = TerrainType::MUD;
  }

  Position start{0, 0};
  Position goal{0, 4};

  comparePathAlgorithms(maze, start, goal);

  auto bfs = findPathBfs(maze, start, goal);
  auto dijkstra = findPathDijkstra(maze, start, goal);

  if (bfs.metrics.pathCost <= dijkstra.metrics.pathCost) {
    throw std::runtime_error("BFS should be more expensive");
  }

  if (!exportPathComparison("data/path_comparison.csv")) {
    throw std::runtime_error("Could not export path comparison CSV");
  }
}
