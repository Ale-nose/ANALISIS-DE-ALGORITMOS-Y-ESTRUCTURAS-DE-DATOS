// Copyright Alejandro, Ashley y Kevin
#include <string>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <fstream>

#include "Maze.hpp"
#include "Pathfinding.hpp"

/**
 * @brief Represents a single benchmark record comparing pathfinding algorithms.
 */
struct PathComparisonRow {
  std::string algorithm;   ///< Name of the evaluated algorithm (BFS, Dijkstra, A*).
  int rows;                ///< Total rows in the evaluated maze grid.
  int cols;                ///< Total columns in the evaluated maze grid.
  std::size_t steps;       ///< Number of movement transitions in the found path.
  int visitedNodes;        ///< Total vertices explored during the search.
  int pathCost;            ///< Accumulated movement cost along the path.
  double cpuTimeMs;        ///< Search duration in milliseconds.
};

/// In-memory storage for path comparison benchmark records
static std::vector<PathComparisonRow> comparisonRows;

/**
 * @brief Executes BFS, Dijkstra, and A* on a maze and logs comparative metrics.
 * @param maze Maze instance containing the graph layout and terrain costs.
 * @param start Origin position coordinate.
 * @param goal Destination position coordinate.
 * @throws std::runtime_error If any algorithm fails or if optimal path costs differ.
 */
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

/**
 * @brief Exports accumulated path comparison metrics into a CSV file.
 * @param filename Target filesystem path for the output CSV.
 * @return true if the file was written successfully, false otherwise.
 */
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

/**
 * @brief Benchmarks algorithm behavior on a dual-path layout with mud penalties.
 * @throws std::runtime_error If Dijkstra fails to beat BFS cost or if CSV export fails.
 */
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

/**
 * @brief Main entry point for path comparison executable.
 * @return Exit status 0 on success, 1 on failure.
 */
int main() {
  try {
    compareMudScenario();
    std::cout << "[INFO] Path comparison successfully exported to data/path_comparison.csv\n";
    return 0;
  } catch (const std::exception& e) {
    std::cerr << "[ERROR] " << e.what() << '\n';
    return 1;
  }
}
