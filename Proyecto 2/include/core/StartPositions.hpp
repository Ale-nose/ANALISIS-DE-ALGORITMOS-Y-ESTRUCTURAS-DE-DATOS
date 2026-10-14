// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <random>

#include "GameTypes.hpp"

struct StartPositions {
  Position player1;
  Position player2;
};

struct StartPositions chooseStartPositions(int rows, int cols, std::mt19937& rng);
