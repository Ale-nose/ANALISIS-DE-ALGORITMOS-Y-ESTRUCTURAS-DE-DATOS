// Copyright Alejandro, Ashley y Kevin
#include <limits>
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>
#include <ctime>
#include <stdexcept>

#include "Pathfinding.hpp"

struct SearchResult findPathDijkstra(const Maze& maze, Position start, Position goal) {
  std::clock_t startTime = std::clock();

  SearchResult result{};

  if (!maze.isInside(start) || !maze.isInside(goal)) {
    throw std::invalid_argument("Start or goal is outside the maze");
  }
  
  int rows = maze.rows();
  int cols = maze.cols();
  int size = rows * cols;

  const int INF = std::numeric_limits<int>::max();
  std::vector<int> distances(size, INF);

  std::vector<int> previous(size, -1);

  using QueueNode = std::pair<int, int>;
  std::priority_queue<QueueNode, std::vector<QueueNode>, std::greater<QueueNode>> pending;

  int startIndex = start.row * cols + start.col;
  int goalIndex = goal.row * cols + goal.col;

  distances[startIndex] = 0;

  pending.push({0, startIndex});

  while (!pending.empty()) {
    int currentCost = pending.top().first;
    int currentIndex = pending.top().second;

    pending.pop();

    if (currentCost != distances[currentIndex]) {
      continue;
    }

    result.metrics.visitedNodes++;

    Position current{currentIndex / cols, currentIndex % cols};

    result.trace.closed.push_back(current);

    if (currentIndex == goalIndex) {
      break;
    }

    for (auto next : maze.neighbors(current)) {
      if (!maze.hasPassage(current, next)) {
        continue;
      }

      int nextIndex = next.row * cols + next.col;
      int newCost = currentCost + maze.entryCost(next);

      if (newCost < distances[nextIndex]) {
        distances[nextIndex] = newCost;
        previous[nextIndex] = currentIndex;
        pending.push({newCost, nextIndex});
        result.trace.opened.push_back(next);
      }
    }
  }

  if (distances[goalIndex] == INF) {
    result.found = false;
  } else {
    result.found = true;
    result.metrics.pathCost = distances[goalIndex];
    int cursor = goalIndex;
    while (cursor != -1) {
      Position position{cursor / cols, cursor % cols};
      result.path.push_back(position);
      cursor = previous[cursor];
    }

    std::reverse(result.path.begin(), result.path.end());
    result.trace.path = result.path;
  }

  std::clock_t endTime = std::clock();

  result.metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return result;
}
