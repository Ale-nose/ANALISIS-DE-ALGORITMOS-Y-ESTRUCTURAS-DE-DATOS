// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <vector>

#include "GameTypes.hpp"
#include "Metrics.hpp"
#include "Maze.hpp"

// Search visualization data
struct SearchTrace {
  std::vector<Position> opened;
  std::vector<Position> closed;
  std::vector<Position> path;
};

// Pathfinding result
struct SearchResult {
  bool found = false;
  std::vector<Position> path;
  SearchMetrics metrics;
  SearchTrace trace;
};

/**
 * @brief Finds the shortest or lowest-cost path between two cells using Dijkstra's algorithm.
 *
 * Explores the maze graph by priority of accumulated cost, accounting for varying terrain
 * entry costs such as mud.
 *
 * @param maze Maze grid to search.
 * @param start Starting cell coordinate.
 * @param goal Target destination cell coordinate.
 * @return SearchResult containing path validity, ordered coordinates, and performance metrics.
 */
struct SearchResult findPathDijkstra(const Maze& maze, Position start, Position goal);

/**
 * @brief Finds a path between two cells using Breadth-First Search (BFS).
 *
 * Traverses the maze level by level, finding a path with the minimal number of steps
 * without accounting for edge weights or terrain costs.
 *
 * @param maze Maze grid to search.
 * @param start Starting cell coordinate.
 * @param goal Target destination cell coordinate.
 * @return SearchResult containing path validity, ordered coordinates, and evaluated metrics.
 */
struct SearchResult findPathBfs(const Maze& maze, Position start, Position goal);


/**
 * @brief Computes the Manhattan distance between two cells.
 *
 * It is the number of rows plus the number of columns that separate them,
 * which is the length of the shortest path on a grid with four-direction
 * movement and no obstacles. It is used as the heuristic of A*.
 *
 * @param a First cell.
 * @param b Second cell.
 * @return The distance, which is never negative. It is 0 only if both cells
 *         are the same.
 */
int manhattanDistance(Position a, Position b);


/**
 * @brief Finds the cheapest path between two cells using A*.
 *
 * The cost of a path is the sum of the entry cost of every cell it enters,
 * without charging the start cell, so mud cells are taken into account. The
 * heuristic is the Manhattan distance to the goal, which never overestimates
 * the real cost when there are no portals or Jump Walls, so the cost found
 * is the minimum one.
 *
 * @param maze Maze to search.
 * @param start Cell where the path begins.
 * @param goal Cell where the path ends.
 * @param saveTrace If true, the cells opened, the cells closed and the final
 *                  path are stored in SearchResult::trace, in order.
 * @return The path from start to goal with its metrics. If the goal cannot
 *         be reached, found is false and the path is empty.
 */
struct SearchResult findPathAStar(const Maze& maze, Position start, Position goal, bool saveTrace = false);
