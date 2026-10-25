// Copyright Alejandro, Ashley y Kevin
#include <stdexcept>

#include "StartPositions.hpp"

struct StartPositions chooseStartPositions(int rows, int cols, std::mt19937& rng) {
  // Ensure grid dimensions are strictly positive
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Invalid argument");
  }

  StartPositions positions{};

  // Uniform distribution to randomly pick a row index within grid limits
  std::uniform_int_distribution<int> randomRow(0, rows - 1);

  // Place Player 1 at a random row on the leftmost boundary (col 0)
  positions.player1.row = randomRow(rng);
  positions.player1.col = 0;

  // Place Player 2 at a random row on the rightmost boundary (cols - 1)
  positions.player2.row = randomRow(rng);
  positions.player2.col = cols - 1;

  return positions;
}
