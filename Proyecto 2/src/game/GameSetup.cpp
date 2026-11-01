// Copyright Alejandro, Ashley y Kevin

#include "GameSetup.hpp"

#include <random>
#include <stdexcept>
#include <utility>

#include "MazeGenerator.hpp"
#include "Pathfinding.hpp"
#include "StartPositions.hpp"
#include "TreasurePlacer.hpp"
#include "WorldSpawner.hpp"

GameState GameSetup::createGame(const GameOptions& options) {
  std::mt19937 rng(options.seed);
  GameState initialState;

  GenerationResult Generation =
      MazeGenerator::generate(options.rows, options.cols, options.generator,
                              options.extraEdgeProb, rng);

  initialState.maze = std::move(Generation.maze);
  StartPositions startingPos =
      chooseStartPositions(options.rows, options.cols, rng);
  initialState.players[PLAYER_1].position = startingPos.player1;
  initialState.players[PLAYER_2].position = startingPos.player2;
  initialState.stats.generation = Generation.metrics;

  WorldSpawner::placeMud(
      initialState.maze, initialState.players[PLAYER_1].position,
      initialState.players[PLAYER_2].position, options.mudSpawnRate, rng);

  SearchResult searchRes = findPathDijkstra(
      initialState.maze, initialState.players[PLAYER_1].position,
      initialState.players[PLAYER_2].position);
  if (!searchRes.found) {
    throw std::runtime_error("Invalid Positions. No path could be found");
  }

  Position treasurePos = chooseTreasurePosition(initialState.maze, searchRes);
  initialState.treasure = treasurePos;
  WorldSpawner::placePortal(
      initialState.maze, initialState.players[PLAYER_1].position,
      initialState.players[PLAYER_2].position, initialState.treasure, rng);

  WorldSpawner::placePowerUp(
      initialState.maze, initialState.players[PLAYER_1].position,
      initialState.players[PLAYER_2].position, initialState.treasure, rng);

  WorldSpawner::placeShieldTile(
      initialState.maze, initialState.players[PLAYER_1].position,
      initialState.players[PLAYER_2].position, initialState.treasure, rng);

  SearchRecord entry;
  entry.algorithm = SearchAlgorithm::DIJKSTRA;
  entry.metrics = searchRes.metrics;

  initialState.stats.searches.push_back(entry);
  initialState.options = options;

  return initialState;
}
