// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>

#include "Pathfinding.hpp"

void testBfsPathfinding() {
  // Test 1: Straight path in a 1x4 horizontal corridor
  Maze maze(1, 4);

  // Open horizontal passages connecting (0,0) through (0,3)
  maze.openPassage({0, 0}, Direction::RIGHT);
  maze.openPassage({0, 1}, Direction::RIGHT);
  maze.openPassage({0, 2}, Direction::RIGHT);

  auto result = findPathBfs(maze, {0, 0}, {0, 3});

  // Verify path reachability, node count, step cost, and endpoints
  assert(result.found);
  assert(result.path.size() == 4);
  assert(result.metrics.pathCost == 3);
  assert((result.path.front() == Position{0, 0}));
  assert((result.path.back() == Position{0, 3}));

  // Test 2: Unreachable destination in a closed maze
  Maze disconnected(2, 2);

  auto result2 = findPathBfs(
    disconnected, {0, 0}, {1, 1}
  );

  // Verify search failure and empty path
  assert(!result2.found);
  assert(result2.path.empty());

  // Test 3: Start position equals target position
  auto result3 = findPathBfs(
    disconnected, {0, 0}, {0, 0}
  );

  // Cost must be 0 and path should only include the initial cell
  assert(result3.found);
  assert(result3.path.size() == 1);
  assert(result3.metrics.pathCost == 0);

  // Test 4: Comparison between BFS (fewest steps) and Dijkstra (cheapest cost) with mud
  Maze mudMaze(2, 5);

  // Connect upper and lower corridors
  for (int col = 0; col < 4; col++) {
    mudMaze.openPassage({0, col}, Direction::RIGHT);
    mudMaze.openPassage({1, col}, Direction::RIGHT);
  }

  // Open vertical transitions at the ends
  mudMaze.openPassage({0, 0}, Direction::DOWN);
  mudMaze.openPassage({0, 4}, Direction::DOWN);

  // Apply high-cost mud terrain to the upper corridor
  mudMaze.cellAt({0, 1}).terrain = TerrainType::MUD;
  mudMaze.cellAt({0, 2}).terrain = TerrainType::MUD;
  mudMaze.cellAt({0, 3}).terrain = TerrainType::MUD;

  auto bfs = findPathBfs(mudMaze, {0, 0}, {0, 4});
  auto dijkstra = findPathDijkstra(mudMaze, {0, 0}, {0, 4});

  // Both algorithms must find a valid route
  assert(bfs.found);
  assert(dijkstra.found);

  // BFS chooses the 5-cell direct path; Dijkstra chooses the 7-cell clean detour
  assert(bfs.path.size() == 5);
  assert(dijkstra.path.size() == 7);

  // BFS incurs higher entry cost (7), while Dijkstra minimizes total cost (6)
  assert(bfs.metrics.pathCost == 7);
  assert(dijkstra.metrics.pathCost == 6);

  std::cout << "All BFS pathfinding tests passed!\n";
}
