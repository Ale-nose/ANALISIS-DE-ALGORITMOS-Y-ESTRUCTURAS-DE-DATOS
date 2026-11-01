// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <random>

#include "GameTypes.hpp"

/**
 * @brief Holds the initial starting coordinates for both players.
 */
struct StartPositions {
  Position player1;  ///< Starting coordinate for Player 1 (column 0).
  Position player2;  ///< Starting coordinate for Player 2 (last column).
};

/**
 * @brief Randomly selects valid starting positions for Player 1 and Player 2.
 *
 * Places Player 1 on a random row in column 0, and Player 2 on a random row
 * in the last column (cols - 1).
 *
 * @param rows Total number of rows in the maze grid.
 * @param cols Total number of columns in the maze grid.
 * @param rng Random number generator engine.
 * @return StartPositions containing the initial coordinates for both players.
 */
struct StartPositions chooseStartPositions(int rows, int cols,
                                           std::mt19937& rng);
