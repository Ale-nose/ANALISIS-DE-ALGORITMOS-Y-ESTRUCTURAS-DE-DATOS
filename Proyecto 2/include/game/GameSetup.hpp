// Copyright Alejandro, Ashley y Kevin

#pragma once

#include "GameState.hpp"

class GameSetup {
 public:
  /**
   * @brief Builds the initial state of a match from the menu options.
   *  1. Generate the maze (DFS or BFS) and add the extra passages.
   *  2. Choose the starting positions of both players.
   *  3. Place the mud.
   *  4. Find the cheapest path between the players with Dijkstra.
   *  5. Place the treasure at the middle of that path's cost.
   *  6. Place portals, power-ups and shield tiles, excluding the starting
   *     positions and the treasure.
   * @param options Match settings: mode, generation algorithm, fog, seed,
   *                    map size, extra edge probability and mud spawn rate.
   * @return The complete initial game state, ready to be handed to the engine.
   *
   * @throws std::runtime_error If no path exists between the two starting
   *         positions, which would leave the treasure undefined.
   */
  static GameState createGame(const GameOptions& options);
};