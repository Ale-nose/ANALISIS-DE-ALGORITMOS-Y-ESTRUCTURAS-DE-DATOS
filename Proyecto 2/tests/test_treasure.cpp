// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>

#include "TreasurePlacer.hpp"

int main() {
  // Test 1: Path with total cost 8
  Maze maze(1, 9);

  for (int col = 0; col < 8; col++) {
    maze.openPassage({0, col}, Direction::RIGHT);
  }

  auto path = findPathDijkstra(maze, {0, 0}, {0, 8});

  Position treasure = chooseTreasurePosition(maze, path);

  assert((treasure == Position{0, 4}));

  // Test 2: Tie between two positions
  Maze maze2(1, 4);

  for (int col = 0; col < 3; col++) {
    maze2.openPassage({0, col}, Direction::RIGHT);
  }

  auto path2 = findPathDijkstra(maze2, {0, 0}, {0, 3});

  Position treasure2 = chooseTreasurePosition(maze2, path2);

  assert((treasure2 == Position{0, 1}));

  // Test 3: Start equals goal
  auto path3 = findPathDijkstra(maze2, {0, 0}, {0, 0});

  Position treasure3 = chooseTreasurePosition(maze2, path3);

  assert((treasure3 == Position{0, 0}));

  std::cout << "All treasure placement tests passed!\n";

  return 0;
}
