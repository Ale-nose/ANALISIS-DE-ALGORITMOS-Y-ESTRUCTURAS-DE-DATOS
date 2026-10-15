// Copyright Alejandro, Ashley y Kevin
#include <vector>
#include <stack>
#include <ctime>
#include <utility>
#include <queue>
#include <array>
#include <algorithm>
#include <stdexcept>
#include <optional>

#include "MazeGenerator.hpp"

GenerationResult MazeGenerator::generateDfs(int rows, int cols, std::mt19937& rng) {
  std::clock_t startTime = std::clock();
  
  Maze maze(rows, cols);
  GenerationMetrics metrics{};
  std::vector<bool> visited(rows * cols, false);
  std::stack<Position> pending;

  Position start{0, 0};
  visited[0] = true;
  metrics.visitedNodes = 1;
  pending.push(start);

  while (!pending.empty()) {
    Position current = pending.top();
    std::vector<Direction> availableDirections;

    for (auto direction : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
      Position next = current;

      if (direction == Direction::UP) {
        next.row--;
      } else if (direction == Direction::DOWN) {
        next.row++;
      } else if (direction == Direction::LEFT) {
        next.col--;
      } else if (direction == Direction::RIGHT) {
        next.col++;
      }

      // Check if the neighbor is inside the maze
      if (!maze.isInside(next)) {
        continue;
      }

      // Calculate the neighbor's index
      int index = next.row * cols + next.col;

      // Add the direction if the neighbor has not been visited
      if (!visited[index]) {
        availableDirections.push_back(direction);
      }
    }

    if (availableDirections.empty()) {
      pending.pop();
      continue;
    }

    std::uniform_int_distribution<std::size_t> dist(0, availableDirections.size() - 1);
    std::size_t randomIndex = dist(rng);
    Direction selectedDirection = availableDirections[randomIndex];

    Position next = current;
    if (selectedDirection == Direction::UP) {
      next.row--;
    } else if (selectedDirection == Direction::DOWN) {
      next.row++;
    } else if (selectedDirection == Direction::LEFT) {
      next.col--;
    } else if (selectedDirection == Direction::RIGHT) {
      next.col++;
    }

    maze.openPassage(current, selectedDirection);

    int index = next.row * cols + next.col;
    visited[index] = true;

    metrics.visitedNodes++;
    metrics.createdEdges++;

    pending.push(next);
  }

  std::clock_t endTime = std::clock();

  metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return GenerationResult{std::move(maze), metrics};
}

GenerationResult MazeGenerator::generateBfs(int rows, int cols, std::mt19937& rng) {
  std::clock_t startTime = std::clock();

  Maze maze(rows, cols);
  GenerationMetrics metrics{};
  std::vector<bool> visited(rows * cols, false);
  std::queue<Position> pending;

  Position start{0, 0};
  visited[0] = true;
  metrics.visitedNodes = 1;
  pending.push(start);

  while (!pending.empty()) {
    Position current = pending.front();
    pending.pop();

    std::array<Direction, 4> directions =
      {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT};
    
    std::shuffle(directions.begin(), directions.end(), rng);

    for (auto direction : {Direction::UP, Direction::DOWN, Direction::LEFT, Direction::RIGHT}) {
      Position next = current;

      if (direction == Direction::UP) {
        next.row--;
      } else if (direction == Direction::DOWN) {
        next.row++;
      } else if (direction == Direction::LEFT) {
        next.col--;
      } else if (direction == Direction::RIGHT) {
        next.col++;
      }

      if (maze.isInside(next) == false) {
        continue;
      }

      int index = next.row * cols + next.col;

      if (visited[index] == true) {
        continue;
      }

      maze.openPassage(current, direction);
      visited[index] = true;
      metrics.visitedNodes++;
      metrics.createdEdges++;
      pending.push(next);
    }
  }
  std::clock_t endTime = std::clock();

  metrics.cpuTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  return GenerationResult{std::move(maze), metrics};
}

int MazeGenerator::addExtraPassages(Maze& maze, double probability, std::mt19937& rng) {
  if (probability < 0 || probability > 1) {
    throw std::invalid_argument("Invalid probability");
  }

  int addedPassages = 0;

  int rows = maze.rows();
  int cols = maze.cols();

  std::bernoulli_distribution shouldOpen(probability);

  for (int row = 0; row < rows; row++) {
    for (int col = 0; col < cols; col++) {
      Position current{row, col};

      for (auto direction : {Direction::RIGHT, Direction::DOWN}) {
        Position next = current;

        if (direction == Direction::RIGHT) {
          next.col++;
        } else if (direction == Direction::DOWN) {
          next.row++;
        }

        if (maze.isInside(next) == false) {
          continue;
        }

        if (maze.hasPassage(current, next) == true) {
          continue;
        }

        if (shouldOpen(rng)) {
          maze.openPassage(current, direction);
          addedPassages++;
        }
      }
    }
  }
  return addedPassages;
}

GenerationResult MazeGenerator::generate(
  int rows, int cols, GenerationAlgorithm algorithm, double extraEdgeProb, std::mt19937& rng) {
  if (extraEdgeProb < 0 || extraEdgeProb > 1) {
    throw std::invalid_argument("Invalid probability");
  }

  std::optional<GenerationResult> result;

  if (algorithm == GenerationAlgorithm::DFS) {
    result = generateDfs(rows, cols, rng);
  } else if (algorithm == GenerationAlgorithm::BFS) {
    result = generateBfs(rows, cols, rng);
  } else {
    throw std::invalid_argument("Invalid algorithm");
  }

  GenerationResult generated = std::move(result.value());

  std::clock_t startTime = std::clock();

  int added = addExtraPassages(generated.maze, extraEdgeProb, rng);

  generated.metrics.extraEdges += added;
  generated.metrics.createdEdges += added;

  std::clock_t endTime = std::clock();

  double extraTimeMs = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;

  generated.metrics.cpuTimeMs += extraTimeMs;

  return generated;
}