// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <random>

#include "GameEngine.hpp"

// Safety limit so a match that never ends cannot block the program
inline constexpr int AUTO_MATCH_MAX_ACTIONS = 20000;

/**
 * @brief Plays a whole match without any user, for tests and benchmarks.
 * Player 1 follows the cheapest A* path to the treasure, recomputed every
 * turn. Player 2 is played by the real AIController, and every search of the
 * AIController is stored in the engine with recordAiSearch.
 * @param engine Engine of a freshly created match.
 * @param rng Random generator used by the AIController.
 * @param maxActions Maximum number of actions before giving up.
 * @return True if the match ended with a winner, false if it was stuck or the
 * limit was reached.
 */
bool playAutomatedMatch(GameEngine& engine, std::mt19937& rng
  , int maxActions = AUTO_MATCH_MAX_ACTIONS);
