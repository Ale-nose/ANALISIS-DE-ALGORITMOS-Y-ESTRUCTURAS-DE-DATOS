// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>
#include <random>

#include "StartPositions.hpp"

int main() {
  std::mt19937 rng(42);

  for (int i = 0; i < 100; i++) {
    auto positions = chooseStartPositions(10, 20, rng);

    assert(positions.player1.col == 0);
    assert(positions.player2.col == 19);

    assert(positions.player1.row >= 0);
    assert(positions.player1.row < 10);

    assert(positions.player2.row >= 0);
    assert(positions.player2.row < 10);
  }

  std::cout << "All start positions tests passed!\n";
  return 0;
}
