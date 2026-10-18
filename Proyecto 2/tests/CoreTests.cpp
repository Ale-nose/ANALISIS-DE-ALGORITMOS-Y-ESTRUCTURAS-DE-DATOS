// Copyright Alejandro, Ashley y Kevin
#include <algorithm>
#include <initializer_list>
#include <random>
#include <stdexcept>
#include <string>

#include "GameConfig.hpp"
#include "Maze.hpp"
#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"
#include "TreasurePlacer.hpp"

void testMazeWalls() {
  Maze maze(2, 3);
  require(countOpenEdges(maze) == 0, "New maze must have all walls closed");
  maze.openPassage({0, 0}, Direction::RIGHT);
  maze.openPassage({0, 1}, Direction::DOWN);
  require(countOpenEdges(maze) == 2, "Exactly two passages should be open");
  require(maze.hasPassage({0, 0}, {0, 1}), "First passage missing");
  require(maze.hasPassage({0, 1}, {1, 1}), "Second passage missing");
  require(!maze.hasPassage({0, 0}, {1, 0}), "An unopened wall must remain closed");
  requireSymmetricPassages(maze);
}

void testDfsConnectivity() {
  std::mt19937 rng(42);
  auto result = MazeGenerator::generate(3, 4, GenerationAlgorithm::DFS, 0.0, rng);
  require(result.metrics.visitedNodes == 12, "DFS must visit all 12 cells");
  require(result.metrics.createdEdges == 11, "DFS tree must have 11 edges");
  require(countOpenEdges(result.maze) == 11, "DFS open edge count is wrong");
  requireSymmetricPassages(result.maze);
  requireConnected(result.maze);
}

void testBfsConnectivity() {
  std::mt19937 rng(42);
  auto result = MazeGenerator::generate(3, 4, GenerationAlgorithm::BFS, 0.0, rng);
  require(result.metrics.visitedNodes == 12, "BFS must visit all 12 cells");
  require(result.metrics.createdEdges == 11, "BFS tree must have 11 edges");
  require(countOpenEdges(result.maze) == 11, "BFS open edge count is wrong");
  requireSymmetricPassages(result.maze);
  requireConnected(result.maze);
}

void testExtraPassages() {
  std::mt19937 rng(101);
  for (auto algorithm : {GenerationAlgorithm::DFS, GenerationAlgorithm::BFS}) {
    auto noExtras = MazeGenerator::generate(3, 4, algorithm, 0.0, rng);
    auto allExtras = MazeGenerator::generate(3, 4, algorithm, 1.0, rng);
    require(noExtras.metrics.extraEdges == 0, "0% must add no extra edges");
    require(noExtras.metrics.createdEdges == 11, "0% should create a tree with 11 edges");
    require(countOpenEdges(noExtras.maze) == 11, "0% open edge count is wrong");
    require(allExtras.metrics.extraEdges == 6, "100% must add six extra edges");
    require(allExtras.metrics.createdEdges == 17, "100% must create all 17 possible edges");
    require(countOpenEdges(allExtras.maze) == 17, "100% open edge count is wrong");
    requireSymmetricPassages(noExtras.maze);
    requireSymmetricPassages(allExtras.maze);
    requireConnected(noExtras.maze);
    requireConnected(allExtras.maze);
    auto shortest = findPathBfs(allExtras.maze, {0, 0}, {2, 3});
    require(shortest.found && shortest.path.size() == 6, "Fully open maze needs a five-step path");
  }
}

void testDijkstraMud() {
  Maze maze = makeTwoRouteMaze();
  auto result = findPathDijkstra(maze, {0, 0}, {0, 4});
  const int directCost = 3 * MUD_COST + NORMAL_COST;
  const int detourCost = 6 * NORMAL_COST;
  
  require(result.found, "Dijkstra did not find the goal");
  require(result.metrics.pathCost == std::min(directCost, detourCost), "Dijkstra chose the wrong cost");
  require(result.path.front() == Position{0, 0}, "Dijkstra path starts at the wrong cell");
  require(result.path.back() == Position{0, 4}, "Dijkstra path ends at the wrong cell");
  if (detourCost < directCost) {
    require(result.path.size() == 7, "Dijkstra should take the six-step detour");
  }
}

void testBfsSteps() {
  Maze maze = makeTwoRouteMaze();
  auto result = findPathBfs(maze, {0, 0}, {0, 4});
  
  require(result.found, "BFS did not find the goal");
  require(result.path.size() == 5, "BFS should choose the four-step direct path");
  require(result.path[1] == Position{0, 1}, "BFS should enter the upper corridor");
  require(result.metrics.pathCost == 3 * MUD_COST + NORMAL_COST, "BFS reported the wrong cost");
}

void testTreasureMidpoint() {
  Maze normal = makeLineMaze(9);
  auto normalPath = findPathDijkstra(normal, {0, 0}, {0, 8});
  require(normalPath.found, "Normal treasure path not found");
  require(chooseTreasurePosition(normal, normalPath) == Position{0, 4}, "Treasure must be at (0, 4)");
  
  Maze weighted = makeLineMaze(7);
  weighted.cellAt({0, 1}).terrain = TerrainType::MUD;
  weighted.cellAt({0, 2}).terrain = TerrainType::MUD;
  auto weightedPath = findPathDijkstra(weighted, {0, 0}, {0, 6});
  require(weightedPath.found, "Weighted treasure path not found");
  require(chooseTreasurePosition(weighted, weightedPath) == Position{0, 2}, "Treasure must use path cost");
}

void require(bool condition, const std::string& message) {
  if (!condition) throw std::runtime_error(message);
}

int countOpenEdges(const Maze& maze) {
  int edges = 0;
  for (int row = 0; row < maze.rows(); row++) {
    for (int col = 0; col < maze.cols(); col++) {
      if (col + 1 < maze.cols() && maze.hasPassage({row, col}, {row, col + 1})) edges++;
      if (row + 1 < maze.rows() && maze.hasPassage({row, col}, {row + 1, col})) edges++;
    }
  }
  return edges;
}

void requireSymmetricPassages(const Maze& maze) {
  for (int row = 0; row < maze.rows(); row++) {
    for (int col = 0; col < maze.cols(); col++) {
      if (col + 1 < maze.cols()) {
        bool forward = maze.hasPassage({row, col}, {row, col + 1});
        bool backward = maze.hasPassage({row, col + 1}, {row, col});
        require(forward == backward, "Horizontal passage is not symmetric");
      }
      if (row + 1 < maze.rows()) {
        bool forward = maze.hasPassage({row, col}, {row + 1, col});
        bool backward = maze.hasPassage({row + 1, col}, {row, col});
        require(forward == backward, "Vertical passage is not symmetric");
      }
    }
  }
}

void requireConnected(const Maze& maze) {
  for (int row = 0; row < maze.rows(); row++) {
    for (int col = 0; col < maze.cols(); col++) {
      auto result = findPathBfs(maze, {0, 0}, {row, col});
      require(result.found, "Unreachable cell (" + std::to_string(row) + ", " + std::to_string(col) + ")");
      require(result.path.front() == Position{0, 0}, "Incorrect path start");
      require(result.path.back() == Position{row, col}, "Incorrect path destination");
    }
  }
}

Maze makeTwoRouteMaze() {
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
  return maze;
}

Maze makeLineMaze(int cols) {
  Maze maze(1, cols);
  for (int col = 0; col + 1 < cols; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
  }
  return maze;
}
