// Copyright Alejandro, Ashley y Kevin
#include <algorithm>
#include <ctime>
#include <functional>
#include <limits>
#include <queue>
#include <stdexcept>
#include <vector>

#include "Pathfinding.hpp"

struct SearchResult findPathDijkstra(const Maze& maze, Position start,
                                     Position goal) {
  std::clock_t startTime = std::clock();

  SearchResult result{};

  // Validate that start and goal positions are within maze boundaries
  if (!maze.isInside(start) || !maze.isInside(goal)) {
    throw std::invalid_argument("Start or goal is outside the maze");
  }

  int rows = maze.rows();
  int cols = maze.cols();
  int size = rows * cols;

  // Initialize distances with infinity and predecessor indices with -1
  const int INF = std::numeric_limits<int>::max();
  std::vector<int> distances(size, INF);

  std::vector<int> previous(size, -1);

  // Min-heap priority queue storing pairs of (cost, cellIndex)
  using QueueNode = std::pair<int, int>;
  std::priority_queue<QueueNode, std::vector<QueueNode>,
                      std::greater<QueueNode>>
      pending;

  int startIndex = start.row * cols + start.col;
  int goalIndex = goal.row * cols + goal.col;

  // Starting cell begins with 0 accumulated cost
  distances[startIndex] = 0;

  pending.push({0, startIndex});

  // Main Dijkstra loop: process the lowest-cost unvisited cell
  while (!pending.empty()) {
    int currentCost = pending.top().first;
    int currentIndex = pending.top().second;

    pending.pop();

    // Skip stale queue entries with higher outdated costs
    if (currentCost != distances[currentIndex]) {
      continue;
    }

    result.metrics.visitedNodes++;

    Position current{currentIndex / cols, currentIndex % cols};

    result.trace.closed.push_back(current);

    // Stop early if target destination is reached
    if (currentIndex == goalIndex) {
      break;
    }

    // Explore all adjacent neighbors
    for (auto next : maze.neighbors(current)) {
      // Ensure wall is open before attempting to traverse
      if (!maze.hasPassage(current, next)) {
        continue;
      }

      int nextIndex = next.row * cols + next.col;
      int newCost = currentCost + maze.entryCost(next);

      // Edge relaxation: update shortest path to neighbor if cheaper
      if (newCost < distances[nextIndex]) {
        distances[nextIndex] = newCost;
        previous[nextIndex] = currentIndex;
        pending.push({newCost, nextIndex});
        result.trace.opened.push_back(next);
      }
    }
  }

  // Reconstruct path if destination was reached
  if (distances[goalIndex] == INF) {
    result.found = false;
  } else {
    result.found = true;
    result.metrics.pathCost = distances[goalIndex];
    int cursor = goalIndex;
    // Trace back using predecessor pointers from goal to start
    while (cursor != -1) {
      Position position{cursor / cols, cursor % cols};
      result.path.push_back(position);
      cursor = previous[cursor];
    }

    // Reverse to restore start-to-goal order
    std::reverse(result.path.begin(), result.path.end());
    result.trace.path = result.path;
  }

  std::clock_t endTime = std::clock();

  // Calculate elapsed CPU execution time in milliseconds
  result.metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return result;
}
