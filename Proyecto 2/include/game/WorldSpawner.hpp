// Copyright Alejandro, Ashley y Kevin

#pragma once

#include <random>
#include "Maze.hpp"
#include "GameTypes.hpp"


/**
 * @brief Utility class that places random world elements on a maze.
 *
 * All functions are static: they keep no state between calls and only
 * modify the maze they receive.
 */
class WorldSpawner {

  public:
  /**
   * @brief Marks random cells of the maze as mud.
   *
   * Visits every cell except the two starting positions and, with
   * probability @p rate, sets its terrain to TerrainType::MUD. Walls are not
   * modified, and the movement cost is not applied here: Maze::entryCost
   * reads it from the terrain.
   *
   * @param maze Maze whose cells may be changed to mud.
   * @param posPlayer1 Starting position of player 1 (never mud).
   * @param posPlayer2 Starting position of player 2 (never mud).
   * @param rate Probability in [0.0, 1.0] that an eligible cell becomes
   *                 mud. 0.0 places no mud and 1.0 fills every eligible cell.
   * @param random Random number generator, shared with the rest of
   *                       the setup so that the same seed produces the same
   *                       maze.
   *
   * @note The cells are visited in a fixed order, which keeps the result
   *       reproducible for a given generator state.
   */
    static void placeMud(Maze& maze, Position posPlayer1, Position posPlayer2, double rate, std::mt19937& random);


};