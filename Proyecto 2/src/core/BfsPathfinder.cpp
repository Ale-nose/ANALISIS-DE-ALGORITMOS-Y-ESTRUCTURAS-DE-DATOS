// Copyright Alejandro, Ashley y Kevin
#include <ctime>
#include <stdexcept>
#include <vector>
#include <queue>
#include <algorithm>

#include "Pathfinding.hpp"

struct SearchResult findPathBfs(const Maze& maze, Position start, Position goal) {
  std::clock_t startTime = std::clock();

  SearchResult result{};

  if (!maze.isInside(start) || !maze.isInside(goal)) {
    throw std::invalid_argument("Invalid argument");
  }

  int rows = maze.rows();
  int cols = maze.cols();
  int size = rows * cols;

  std::vector<bool> visited(size, false);
  std::vector<int> previous(size, -1);
  std::queue<int> pending;

  int startIndex = start.row * cols + start.col;
  int goalIndex = goal.row * cols + goal.col;

  visited[startIndex] = true;
  pending.push(startIndex);

  result.trace.opened.push_back(start);

  while (!pending.empty()) {
    int currentIndex = pending.front();
    pending.pop();

    Position current{currentIndex / cols, currentIndex % cols};

    result.metrics.visitedNodes++;
    result.trace.closed.push_back(current);

    if (currentIndex == goalIndex) {
      break;
    }

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

  if (visited[goalIndex] == false) {
    result.found = false;
  } else {
    result.found = true;
    int cursor = goalIndex;
  
    while (cursor != -1) {
      Position position{cursor / cols, cursor % cols};
      result.path.push_back(position);
      cursor = previous[cursor]; 
    }
    
    std::reverse(result.path.begin(), result.path.end());

    result.trace.path = result.path;

    int totalCost = 0;
    for (std::size_t i = 1; i < result.path.size(); i++) {
      Position current = result.path[i];
      totalCost += maze.entryCost(current);
    }
    result.metrics.pathCost = totalCost;
  }

  std::clock_t endTime = std::clock();

  result.metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return result;
}
