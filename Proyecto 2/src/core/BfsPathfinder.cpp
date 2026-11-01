// Copyright Alejandro, Ashley y Kevin
#include <algorithm>
#include <ctime>
#include <queue>
#include <stdexcept>
#include <vector>

#include "Pathfinding.hpp"

struct SearchResult findPathBfs(const Maze& maze, Position start,
                                Position goal) {
  std::clock_t startTime = std::clock();

  SearchResult result{};

  // Validate that both start and destination points are within bounds
  if (!maze.isInside(start) || !maze.isInside(goal)) {
    throw std::invalid_argument("Invalid argument");
  }

  int rows = maze.rows();
  int cols = maze.cols();
  int size = rows * cols;

  // Track visited cells, path parent pointers, and frontier queue
  std::vector<bool> visited(size, false);
  std::vector<int> previous(size, -1);
  std::queue<int> pending;

  int startIndex = start.row * cols + start.col;
  int goalIndex = goal.row * cols + goal.col;

  // Initialize search from the starting node
  visited[startIndex] = true;
  pending.push(startIndex);

  result.trace.opened.push_back(start);

  // Process nodes in breadth-first order (FIFO)
  while (!pending.empty()) {
    int currentIndex = pending.front();
    pending.pop();

    Position current{currentIndex / cols, currentIndex % cols};

    result.metrics.visitedNodes++;
    result.trace.closed.push_back(current);

    // Early exit once the goal cell is reached
    if (currentIndex == goalIndex) {
      break;
    }

    // Explore accessible adjacent neighbors
    for (auto next : maze.neighbors(current)) {
      if (!maze.hasPassage(current, next)) {
        continue;
      }

      int nextIndex = next.row * cols + next.col;
      if (visited[nextIndex] == false) {
        visited[nextIndex] = true;
        previous[nextIndex] = currentIndex;
        pending.push(nextIndex);
        result.trace.opened.push_back(next);
      }
    }
  }

  // Verify if a path was successfully found
  if (visited[goalIndex] == false) {
    result.found = false;
  } else {
    result.found = true;
    int cursor = goalIndex;

    // Backtrack from goal to start using parent pointers
    while (cursor != -1) {
      Position position{cursor / cols, cursor % cols};
      result.path.push_back(position);
      cursor = previous[cursor];
    }

    // Reverse to obtain chronological start-to-goal path order
    std::reverse(result.path.begin(), result.path.end());

    result.trace.path = result.path;

    // Calculate accumulated entry cost along the reconstructed path
    int totalCost = 0;
    for (std::size_t i = 1; i < result.path.size(); i++) {
      Position current = result.path[i];
      totalCost += maze.entryCost(current);
    }
    result.metrics.pathCost = totalCost;
  }

  std::clock_t endTime = std::clock();

  // Compute CPU execution time in milliseconds
  result.metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return result;
}
