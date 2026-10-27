// Copyright Alejandro, Ashley y Kevin
#include <cassert>
#include <iostream>
#include <random>

#include "StartPositions.hpp"

void testStartPositions() {
  // Seeded random engine to test reproducibility across iterations
  std::mt19937 rng(42);

  // Verify starting constraints over 100 randomized placements
  for (int i = 0; i < 100; i++) {
    auto positions = chooseStartPositions(10, 20, rng);

    // Player 1 must always be placed on the leftmost column (col 0)
    assert(positions.player1.col == 0);
    // Player 2 must always be placed on the rightmost column (col 19)
    assert(positions.player2.col == 19);

    // Verify Player 1 row coordinate stays within grid boundaries
    assert(positions.player1.row >= 0);
    assert(positions.player1.row < 10);

    // Verify Player 2 row coordinate stays within grid boundaries
    assert(positions.player2.row >= 0);
    assert(positions.player2.row < 10);
  }

  std::cout << "All start positions tests passed!\n";
}
