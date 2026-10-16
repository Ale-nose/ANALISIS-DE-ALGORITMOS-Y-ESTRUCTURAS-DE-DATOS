// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>

#include "Pathfinding.hpp"

int main() {
  // Test 1: Straight path
  Maze maze(1, 4);

  maze.openPassage({0, 0}, Direction::RIGHT);
  maze.openPassage({0, 1}, Direction::RIGHT);
  maze.openPassage({0, 2}, Direction::RIGHT);

  auto result = findPathBfs(maze, {0, 0}, {0, 3});

  assert(result.found);
  assert(result.path.size() == 4);
  assert(result.metrics.pathCost == 3);
  assert((result.path.front() == Position{0, 0}));
  assert((result.path.back() == Position{0, 3}));

  // Test 2: Disconnected maze
  Maze disconnected(2, 2);

  auto result2 = findPathBfs(
    disconnected, {0, 0}, {1, 1}
  );

  assert(!result2.found);
  assert(result2.path.empty());

  // Test 3: Start equals goal
  auto result3 = findPathBfs(
    disconnected, {0, 0}, {0, 0}
  );

  assert(result3.found);
  assert(result3.path.size() == 1);
  assert(result3.metrics.pathCost == 0);

  // Test 4: BFS vs Dijkstra with mud
  Maze mudMaze(2, 5);

  for (int col = 0; col < 4; col++) {
    mudMaze.openPassage({0, col}, Direction::RIGHT);
    mudMaze.openPassage({1, col}, Direction::RIGHT);
  }

  mudMaze.openPassage({0, 0}, Direction::DOWN);
  mudMaze.openPassage({0, 4}, Direction::DOWN);

  mudMaze.cellAt({0, 1}).terrain = TerrainType::MUD;
  mudMaze.cellAt({0, 2}).terrain = TerrainType::MUD;
  mudMaze.cellAt({0, 3}).terrain = TerrainType::MUD;

  auto bfs = findPathBfs(mudMaze, {0, 0}, {0, 4});
  auto dijkstra = findPathDijkstra(mudMaze, {0, 0}, {0, 4});

  assert(bfs.found);
  assert(dijkstra.found);

  assert(bfs.path.size() == 5);
  assert(dijkstra.path.size() == 7);

  assert(bfs.metrics.pathCost == 7);
  assert(dijkstra.metrics.pathCost == 6);

  std::cout << "All BFS pathfinding tests passed!\n";

  return 0;
}