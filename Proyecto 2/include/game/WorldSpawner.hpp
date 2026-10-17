// Copyright Alejandro, Ashley y Kevin

#pragma once

#include <random>
#include <array>
#include <vector>
#include <algorithm>

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

      /**
   * @brief Places shield tiles on random cells of the maze.
   *
   * Visits every cell except the two starting positions and the treasure and,
   * with probability SHIELD_SPAWN_RATE, marks it as SpecialCellType::SHIELD_TILE.
   * Walls and other cell data are not modified, and no shield is granted here:
   * the protection logic is handled by the game engine when a player enters
   * the cell.
   *
   * @param maze Maze whose cells may receive a shield tile.
   * @param posPlayer1 Starting position of player 1 (never a shield tile).
   * @param posPlayer2 Starting position of player 2 (never a shield tile).
   * @param treasure Treasure position (never a shield tile).
   * @param random Random number generator, shared with the rest of the
   *                       setup so that the same seed produces the same maze.
   *
   * @note The cells are visited in a fixed order, which keeps the result
   *       reproducible for a given generator state.
   */
  static void placeShieldTile(Maze& maze,Position posPlayer1,Position posPlayer2, Position treasure,std::mt19937& random);

    /**
   * @brief Places random power-ups on the maze.
   *
   * Visits every cell except the two starting positions and the treasure and,
   * with probability POWER_SPAWN_RATE, stores one of the three power-ups
   * (PowerUpType::JUMP_WALL, PowerUpType::DOUBLE_PLAY or
   * PowerUpType::CONTROL_ENEMY) chosen uniformly at random. Walls and other
   * cell data are not modified, and no player inventory or turn is changed
   * here: the power-up is applied by the game engine when a player enters
   * the cell.
   *
   * @param[in,out] maze Maze whose cells may receive a power-up.
   * @param[in] posPlayer1 Starting position of player 1 (never gets a power-up).
   * @param[in] posPlayer2 Starting position of player 2 (never gets a power-up).
   * @param[in] treasure Treasure position (never gets a power-up).
   * @param[in,out] random Random number generator, shared with the rest of the
   *                       setup so that the same seed produces the same maze.
   *
   * @note The cells are visited in a fixed order, and the second draw (which
   *       power-up) only happens when the first one succeeds, which keeps the
   *       result reproducible for a given generator state.
   */
  static void placePowerUp(Maze& maze, Position posPlayer1,Position posPlayer2, Position treasure,std::mt19937& random);

  /**
   * @brief Places portals in pairs on random cells of the maze.
   *
   * Visits every cell except the two starting positions and the treasure and,
   * with probability PORTAL_SPAWN_RATE, selects it as a portal candidate. The
   * candidates are shuffled and grouped two by two: both cells of a pair
   * receive the same positive identifier. If the number of candidates is odd,
   * the last one is left without a portal. Cells without a portal keep their
   * default value. Nobody is teleported here: that is handled by the game
   * engine when a player enters a portal.
   *
   * @param maze Maze whose cells may receive a portal identifier.
   * @param posPlayer1 Starting position of player 1 (never a portal).
   * @param posPlayer2 Starting position of player 2 (never a portal).
   * @param treasure Treasure position (never a portal).
   * @param random Random number generator, shared with the rest of the
   *                       setup so that the same seed produces the same maze.
   *
   * @note The shuffle makes the pairs spread over the whole maze instead of
   *       linking cells that are close in scan order. The result is
   *       reproducible for a given generator state and standard library.
   */
  static void placePortal(Maze& maze, Position posPlayer1,Position posPlayer2, Position treasure,std::mt19937& random);



};