// Copyright Alejandro, Ashley y Kevin
#include <limits>
#include <queue>
#include <vector>
#include <functional>
#include <algorithm>
#include <ctime>
#include <stdexcept>

#include "Pathfinding.hpp"

int manhattanDistance(Position a, Position b) {
  int distancesRow = std::abs(a.row - b.row);
  int distancesCol = std::abs(a.col - b.col);
  int totalDist = std::abs(distancesCol+distancesRow);
  return totalDist;
}

SearchResult findPathAStar(const Maze& maze, Position start, Position goal, bool saveTrace) {
  SearchResult result{};

  std::clock_t startTime = std::clock();
  const int INFINITECOST = std::numeric_limits<int>::max();
  const int NO_PREDECESSOR = -1;
  int mazeSizeInt = maze.rows() * maze.cols();
  std::size_t mazeSize = static_cast<std::size_t>(mazeSizeInt);

  std::vector<int> g(mazeSize, INFINITECOST);
  std::vector<int> predecesor(mazeSize, NO_PREDECESSOR);
  std::vector<bool> closedCellIndicator(mazeSize, false);

  auto toIndex = [&maze](Position posToIndex) {
    int index = posToIndex.row * maze.cols() + posToIndex.col; // turns a position into an index for the vectors
    return index;
  };

  auto toPosition = [&maze](int index) {
    int row = index / maze.cols(); // turns index back into Position
    int col = index % maze.cols();
    return Position{row, col};
  };

  using entry = std::pair<int, int>;

  std::priority_queue<entry, std::vector<entry>, std::greater<entry>> minheap;

  int startIndex = toIndex(start);
  int goalIndex = toIndex(goal);
  g[startIndex] = 0;
  minheap.push({manhattanDistance(start, goal), startIndex});
  if (saveTrace) {
    result.trace.opened.push_back(start);
  }
  int visitedNodes = 0;

  while (!minheap.empty()) {
    int lesser = minheap.top().second;
    minheap.pop();
    if (closedCellIndicator[lesser]) {
      continue;
    }
    closedCellIndicator[lesser] = true;
    visitedNodes++;
    Position currentPosition = toPosition(lesser);
    if (saveTrace) {
      result.trace.closed.push_back(currentPosition);
       // Extra: animated A* search visualizer
      result.trace.openedCountAtClose.push_back(result.trace.opened.size());
    }

    if (lesser == goalIndex) {
      break;
    }

    for (const Position& next : maze.neighbors(currentPosition)) {
      int nextIndex = toIndex(next);
      int newG = g[lesser] + maze.entryCost(next);
      if (newG < g[nextIndex]) {
        g[nextIndex] = newG;
        predecesor[nextIndex] = lesser;
        minheap.push({newG + manhattanDistance(next, goal), nextIndex});
        if (saveTrace) {
          result.trace.opened.push_back(next);
        }
      }
    }
  }

  if (closedCellIndicator[goalIndex]) {
    result.found = true;
    int step = goalIndex;
    while (step != NO_PREDECESSOR) {
      result.path.push_back(toPosition(step));
      step = predecesor[step];
    }
    std::reverse(result.path.begin(), result.path.end());
    result.metrics.pathCost = g[goalIndex];
    if (saveTrace) {
      result.trace.path = result.path;
    }
  }

  result.metrics.visitedNodes = visitedNodes;
  result.metrics.cpuTimeMs = 1000.0 * static_cast<double>(std::clock() - startTime) / CLOCKS_PER_SEC;
  return result;
}