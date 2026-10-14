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

  auto result = findPathDijkstra(maze, {0, 0}, {0, 3});

  assert(result.found);
  assert(result.metrics.pathCost == 3);
  assert(result.path.size() == 4);
  assert((result.path.front() == Position{0, 0}));
  assert((result.path.back() == Position{0, 3}));

  // Test 2: No available path
  Maze disconnected(2, 2);

  auto result2 = findPathDijkstra(disconnected, {0, 0}, {1, 1});

  assert(!result2.found);
  assert(result2.path.empty());

  // Test 3: Start equals goal
  auto result3 = findPathDijkstra(disconnected, {0, 0}, {0, 0});

  assert(result3.found);
  assert(result3.metrics.pathCost == 0);
  assert(result3.path.size() == 1);

  std::cout << "All Dijkstra tests passed!\n";

  return 0;
}
