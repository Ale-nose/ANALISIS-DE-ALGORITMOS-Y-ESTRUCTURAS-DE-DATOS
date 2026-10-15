// Copyright Alejandro, Ashley y Kevin
#include <stdexcept>

#include "StartPositions.hpp"

struct StartPositions chooseStartPositions(int rows, int cols, std::mt19937& rng) {
  if (rows <= 0 || cols <= 0) {
    throw std::invalid_argument("Invalid argument");
  }

  StartPositions positions{};

  std::uniform_int_distribution<int> randomRow(0, rows - 1);

  positions.player1.row = randomRow(rng);
  positions.player1.col = 0;

  positions.player2.row = randomRow(rng);
  positions.player2.col = cols - 1;

  return positions;
}
