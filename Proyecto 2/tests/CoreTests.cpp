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

// Forward declarations for helper functions used in tests
void require(bool condition, const std::string& message);
int countOpenEdges(const Maze& maze);
void requireSymmetricPassages(const Maze& maze);
void requireConnected(const Maze& maze);
Maze makeTwoRouteMaze();
Maze makeLineMaze(int cols);

// Verifies that opening walls updates bidirectional passages correctly
void testMazeWalls() {
  Maze maze(2, 3);
  // Ensure all walls start closed
  require(countOpenEdges(maze) == 0, "New maze must have all walls closed");
  // Open two passages
  maze.openPassage({0, 0}, Direction::RIGHT);
  maze.openPassage({0, 1}, Direction::DOWN);
  // Check edge count and connectivity
  require(countOpenEdges(maze) == 2, "Exactly two passages should be open");
  require(maze.hasPassage({0, 0}, {0, 1}), "First passage missing");
  require(maze.hasPassage({0, 1}, {1, 1}), "Second passage missing");
  require(!maze.hasPassage({0, 0}, {1, 0}), "An unopened wall must remain closed");
  // Verify walls are open in both directions
  requireSymmetricPassages(maze);
}

// Verifies that DFS visits all cells and generates a spanning tree
void testDfsConnectivity() {
  std::mt19937 rng(42);
  auto result = MazeGenerator::generate(3, 4, GenerationAlgorithm::DFS, 0.0, rng);
  // A spanning tree of 12 cells must have 11 edges
  require(result.metrics.visitedNodes == 12, "DFS must visit all 12 cells");
  require(result.metrics.createdEdges == 11, "DFS tree must have 11 edges");
  require(countOpenEdges(result.maze) == 11, "DFS open edge count is wrong");
  requireSymmetricPassages(result.maze);
  // Ensure every cell is reachable from (0, 0)
  requireConnected(result.maze);
}

// Verifies that BFS visits all cells and generates a spanning tree
void testBfsConnectivity() {
  std::mt19937 rng(42);
  auto result = MazeGenerator::generate(3, 4, GenerationAlgorithm::BFS, 0.0, rng);
  // A spanning tree of 12 cells must have 11 edges
  require(result.metrics.visitedNodes == 12, "BFS must visit all 12 cells");
  require(result.metrics.createdEdges == 11, "BFS tree must have 11 edges");
  require(countOpenEdges(result.maze) == 11, "BFS open edge count is wrong");
  requireSymmetricPassages(result.maze);
  // Ensure every cell is reachable from (0, 0)
  requireConnected(result.maze);
}

// Tests extra passage creation at 0% (no loops) and 100% (all internal walls open)
void testExtraPassages() {
  std::mt19937 rng(101);
  for (auto algorithm : {GenerationAlgorithm::DFS, GenerationAlgorithm::BFS}) {
    auto noExtras = MazeGenerator::generate(3, 4, algorithm, 0.0, rng);
    auto allExtras = MazeGenerator::generate(3, 4, algorithm, 1.0, rng);
    // 0% extra probability leaves exactly 11 edges
    require(noExtras.metrics.extraEdges == 0, "0% must add no extra edges");
    require(noExtras.metrics.createdEdges == 11, "0% should create a tree with 11 edges");
    require(countOpenEdges(noExtras.maze) == 11, "0% open edge count is wrong");
    // 100% extra probability opens all 6 remaining internal walls (17 total edges)
    require(allExtras.metrics.extraEdges == 6, "100% must add six extra edges");
    require(allExtras.metrics.createdEdges == 17, "100% must create all 17 possible edges");
    require(countOpenEdges(allExtras.maze) == 17, "100% open edge count is wrong");
    requireSymmetricPassages(noExtras.maze);
    requireSymmetricPassages(allExtras.maze);
    requireConnected(noExtras.maze);
    requireConnected(allExtras.maze);
    // Verify the shortest Manhattan path length on the fully opened grid
    auto shortest = findPathBfs(allExtras.maze, {0, 0}, {2, 3});
    require(shortest.found && shortest.path.size() == 6, "Fully open maze needs a five-step path");
  }
}

// Verifies that Dijkstra chooses the cheapest path considering mud cost
void testDijkstraMud() {
  Maze maze = makeTwoRouteMaze();
  auto result = findPathDijkstra(maze, {0, 0}, {0, 4});
  const int directCost = 3 * MUD_COST + NORMAL_COST; // Upper muddy route
  const int detourCost = 6 * NORMAL_COST;            // Lower clean detour route
  
  require(result.found, "Dijkstra did not find the goal");
  require(result.metrics.pathCost == std::min(directCost, detourCost), "Dijkstra chose the wrong cost");
  require(result.path.front() == Position{0, 0}, "Dijkstra path starts at the wrong cell");
  require(result.path.back() == Position{0, 4}, "Dijkstra path ends at the wrong cell");
  // Ensure detour path is selected when it is cheaper
  if (detourCost < directCost) {
    require(result.path.size() == 7, "Dijkstra should take the six-step detour");
  }
}

// Verifies that BFS minimizes step count regardless of mud cost
void testBfsSteps() {
  Maze maze = makeTwoRouteMaze();
  auto result = findPathBfs(maze, {0, 0}, {0, 4});
  
  require(result.found, "BFS did not find the goal");
  // BFS must take the shorter 4-step path through mud
  require(result.path.size() == 5, "BFS should choose the four-step direct path");
  require(result.path[1] == Position{0, 1}, "BFS should enter the upper corridor");
  // Verify the real cost accumulated along the chosen path
  require(result.metrics.pathCost == 3 * MUD_COST + NORMAL_COST, "BFS reported the wrong cost");
}

// Verifies treasure placement at the accumulated cost midpoint of Dijkstra's path
void testTreasureMidpoint() {
  // Test on a uniform grid where step count equals cost
  Maze normal = makeLineMaze(9);
  auto normalPath = findPathDijkstra(normal, {0, 0}, {0, 8});
  require(normalPath.found, "Normal treasure path not found");
  require(chooseTreasurePosition(normal, normalPath) == Position{0, 4}, "Treasure must be at (0, 4)");
  
  // Test on a path with mud where cost midpoint shifts
  Maze weighted = makeLineMaze(7);
  weighted.cellAt({0, 1}).terrain = TerrainType::MUD;
  weighted.cellAt({0, 2}).terrain = TerrainType::MUD;
  auto weightedPath = findPathDijkstra(weighted, {0, 0}, {0, 6});
  require(weightedPath.found, "Weighted treasure path not found");
  require(chooseTreasurePosition(weighted, weightedPath) == Position{0, 2}, "Treasure must use path cost");
}

// Asserts a test condition and throws an exception on failure
void require(bool condition, const std::string& message) {
  if (!condition) throw std::runtime_error(message);
}

// Counts unique open passages across the entire maze
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

// Checks that every passage is open symmetrically in both directions
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

// Verifies that all cells are reachable from the starting cell (0, 0)
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

// Creates a test maze with two parallel routes (upper muddy, lower normal)
Maze makeTwoRouteMaze() {
  Maze maze(2, 5);
  for (int col = 0; col < 4; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
    maze.openPassage({1, col}, Direction::RIGHT);
  }
  maze.openPassage({0, 0}, Direction::DOWN);
  maze.openPassage({0, 4}, Direction::DOWN);
  // Add mud tiles to the upper route
  for (int col = 1; col <= 3; col++) {
    maze.cellAt({0, col}).terrain = TerrainType::MUD;
  }
  return maze;
}

// Creates a simple straight-line 1xN maze
Maze makeLineMaze(int cols) {
  Maze maze(1, cols);
  for (int col = 0; col + 1 < cols; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
  }
  return maze;
}
