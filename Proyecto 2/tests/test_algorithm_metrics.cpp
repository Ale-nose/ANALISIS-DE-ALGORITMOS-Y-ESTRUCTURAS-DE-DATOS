// Copyright Alejandro, Ashley y Kevin
#include <cmath>
#include <cstddef>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>

#include "GameConfig.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"

void require(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void checkTime(double timeMs) {
  require(std::isfinite(timeMs) && timeMs >= 0.0, "Invalid CPU time");
}

void checkGeneration(const GenerationResult& result, int rows, int cols, int expectedExtras) {
  const auto& metrics = result.metrics;
  int vertices = rows * cols;
  int maximumEdges = rows * (cols - 1) + cols * (rows - 1);

  require(metrics.visitedNodes == vertices, "Incorrect generation visited nodes");
  require(metrics.createdEdges == vertices - 1 + metrics.extraEdges, "Incorrect created edges");
  require(metrics.createdEdges <= maximumEdges, "Too many created edges");
  require(metrics.extraEdges == expectedExtras, "Incorrect extra edges");
  checkTime(metrics.cpuTimeMs);
}

void testGenerationMetrics() {
  std::mt19937 rng(42);
  int rows = 3;
  int cols = 4;
  int maximumEdges = rows * (cols - 1) + cols * (rows - 1);
  int expectedExtras = maximumEdges - (rows * cols - 1);

  auto dfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, 0.0, rng);
  auto bfs = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, 0.0, rng);
  auto dfsExtra = MazeGenerator::generate(rows, cols, GenerationAlgorithm::DFS, 1.0, rng);
  auto bfsExtra = MazeGenerator::generate(rows, cols, GenerationAlgorithm::BFS, 1.0, rng);

  checkGeneration(dfs, rows, cols, 0);
  checkGeneration(bfs, rows, cols, 0);
  checkGeneration(dfsExtra, rows, cols, expectedExtras);
  checkGeneration(bfsExtra, rows, cols, expectedExtras);

  auto single = MazeGenerator::generate(1, 1, GenerationAlgorithm::DFS, 1.0, rng);
  checkGeneration(single, 1, 1, 0);

  std::cout << "[PASS] Generation metrics\n";
}

void checkPath(const Maze& maze, const SearchResult& result, Position start, Position goal) {
  require(result.found && !result.path.empty(), "Path not found");
  require(result.path.front() == start, "Incorrect starting position");
  require(result.path.back() == goal, "Incorrect goal position");
  require(result.metrics.visitedNodes >= 1, "No visited nodes");
  require(result.metrics.visitedNodes <= maze.rows() * maze.cols(), "Too many visited nodes");
  require(result.trace.closed.size() == static_cast<std::size_t>(result.metrics.visitedNodes), "Invalid trace");

  checkTime(result.metrics.cpuTimeMs);

  int realCost = 0;

  for (std::size_t i = 1; i < result.path.size(); i++) {
    require(maze.hasPassage(result.path[i - 1], result.path[i]), "Path crosses a wall");
    realCost += maze.entryCost(result.path[i]);
  }

  require(result.metrics.pathCost == realCost, "Incorrect path cost");
}

void testSearchMetrics() {
  Maze maze(1, 4);

  for (int col = 0; col < 3; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
  }

  auto bfs = findPathBfs(maze, {0, 0}, {0, 3});
  auto dijkstra = findPathDijkstra(maze, {0, 0}, {0, 3});
  auto astar = findPathAStar(maze, {0, 0}, {0, 3}, true);

  checkPath(maze, bfs, {0, 0}, {0, 3});
  checkPath(maze, dijkstra, {0, 0}, {0, 3});
  checkPath(maze, astar, {0, 0}, {0, 3});

  require(bfs.metrics.visitedNodes == 4, "Incorrect BFS visited nodes");
  require(dijkstra.metrics.visitedNodes == 4, "Incorrect Dijkstra visited nodes");
  require(bfs.metrics.pathCost == 3 * NORMAL_COST, "Incorrect BFS cost");
  require(astar.metrics.pathCost == dijkstra.metrics.pathCost, "A* and Dijkstra costs do not match");

  auto sameBfs = findPathBfs(maze, {0, 0}, {0, 0});
  auto sameDijkstra = findPathDijkstra(maze, {0, 0}, {0, 0});
  auto sameAstar = findPathAStar(maze, {0, 0}, {0, 0}, true);

  checkPath(maze, sameBfs, {0, 0}, {0, 0});
  checkPath(maze, sameDijkstra, {0, 0}, {0, 0});
  checkPath(maze, sameAstar, {0, 0}, {0, 0});

  require(sameBfs.metrics.pathCost == 0, "Incorrect BFS starting cost");
  require(sameDijkstra.metrics.pathCost == 0, "Incorrect Dijkstra starting cost");
  require(sameAstar.metrics.pathCost == 0, "Incorrect A* starting cost");

  Maze disconnected(2, 2);

  auto missingBfs = findPathBfs(disconnected, {0, 0}, {1, 1});
  auto missingDijkstra = findPathDijkstra(disconnected, {0, 0}, {1, 1});
  auto missingAstar = findPathAStar(disconnected, {0, 0}, {1, 1}, true);

  require(!missingBfs.found && !missingDijkstra.found && !missingAstar.found, "Disconnected maze has a path");
  require(missingBfs.metrics.visitedNodes == 1, "Incorrect disconnected BFS metrics");
  require(missingDijkstra.metrics.visitedNodes == 1, "Incorrect disconnected Dijkstra metrics");
  require(missingAstar.metrics.visitedNodes == 1, "Incorrect disconnected A* metrics");

  checkTime(missingBfs.metrics.cpuTimeMs);
  checkTime(missingDijkstra.metrics.cpuTimeMs);
  checkTime(missingAstar.metrics.cpuTimeMs);

  std::cout << "[PASS] Search metrics\n";
}

void testMudMetrics() {
  Maze maze(2, 5);

  for (int col = 0; col < 4; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
    maze.openPassage({1, col}, Direction::RIGHT);
  }

  maze.openPassage({0, 0}, Direction::DOWN);
  maze.openPassage({0, 4}, Direction::DOWN);

  for (int col = 1; col <= 3; col++) {
    maze.cellAt({0, col}).terrain = TerrainType::MUD;
  }

  auto bfs = findPathBfs(maze, {0, 0}, {0, 4});
  auto dijkstra = findPathDijkstra(maze, {0, 0}, {0, 4});
  auto astar = findPathAStar(maze, {0, 0}, {0, 4}, true);

  checkPath(maze, bfs, {0, 0}, {0, 4});
  checkPath(maze, dijkstra, {0, 0}, {0, 4});
  checkPath(maze, astar, {0, 0}, {0, 4});

  require(bfs.path.size() == 5, "BFS did not select the shortest route");
  require(bfs.metrics.pathCost == 3 * MUD_COST + NORMAL_COST, "Incorrect BFS mud cost");
  require(dijkstra.metrics.pathCost <= bfs.metrics.pathCost, "Dijkstra selected a more expensive route");
  require(astar.metrics.pathCost == dijkstra.metrics.pathCost, "A* did not find the minimum-cost route");

  std::cout << "[PASS] Mud metrics\n";
}

int main() {
  try {
    testGenerationMetrics();
    testSearchMetrics();
    testMudMetrics();

    std::cout << "All algorithm metrics tests passed!\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "[FAIL] " << error.what() << '\n';
    return 1;
  }
}
