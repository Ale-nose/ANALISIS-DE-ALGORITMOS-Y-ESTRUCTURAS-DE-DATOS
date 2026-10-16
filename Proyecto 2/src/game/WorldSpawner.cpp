// Copyright Alejandro, Ashley y Kevin

#include "WorldSpawner.hpp"

void WorldSpawner::placeMud (Maze& maze, Position posPlayer1, Position posPlayer2, double rate, std::mt19937& random) {
  std::bernoulli_distribution chance(rate);
  for (int i = 0; i < maze.rows(); i++) {
    for (int j = 0; j < maze.cols(); j++) {
      Position current {i,j};
      if (current == posPlayer1 || current == posPlayer2) {
        //skips it 
      } else {
        if(chance(random)) {
          maze.cellAt(current).terrain = TerrainType::MUD;
        }
      }
    }
    
  }
  

}