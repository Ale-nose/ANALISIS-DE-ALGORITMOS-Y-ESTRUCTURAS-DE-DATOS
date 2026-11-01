// Copyright Alejandro, Ashley y Kevin
#pragma once

#include "Maze.hpp"
#include "Pathfinding.hpp"

/**
 * @brief Selects the treasure position along the shortest path between players.
 *
 * Traverses the path calculated by Dijkstra and chooses the cell whose
 * accumulated entry cost is closest to half of the total path cost.
 *
 * @param maze Maze grid used to evaluate cell entry costs.
 * @param shortestPath Result of Dijkstra's algorithm between Player 1 and
 * Player 2.
 * @return Position of the chosen cell where the treasure will be placed.
 */
Position chooseTreasurePosition(const Maze& maze,
                                const SearchResult& shortestPath);
