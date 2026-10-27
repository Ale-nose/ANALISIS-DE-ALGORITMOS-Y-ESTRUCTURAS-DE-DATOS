// Copyright Alejandro, Ashley y Kevin
#include <string>
#include <utility>
#include <vector>

#include "GameConfig.hpp"
#include "GameEngine.hpp"
#include "GameTypes.hpp"
#include "Pathfinding.hpp"
#include "Visibility.hpp"

static_assert(MUD_COST == 2 && ACTIONS_PER_TURN == 1,
              "These tests assume MUD_COST = 2 and ACTIONS_PER_TURN = 1");

// Defined in CoreTests.cpp: throws a runtime_error when the condition fails
void require(bool condition, const std::string& message);

// Forward declarations for helper functions used in tests
GameState makeTestState();
Action moveAction(Direction direction);
Action jumpAction(Direction direction);
bool contains(const std::vector<Position>& cells, Position target);

// Verifies that illegal moves are rejected without any cost
void testBlockedMove() {
  GameEngine engine(makeTestState());
  // A wall blocks the move downwards
  require(!engine.submitAction(moveAction(Direction::DOWN)), "A move into a wall must be rejected");
  require(engine.state().players[PLAYER_1].position == Position{0, 0}, "A rejected move must keep the position");
  require(engine.currentPlayer() == PLAYER_1, "A rejected move must keep the turn");
  require(engine.state().actionsLeft == ACTIONS_PER_TURN, "A rejected move must keep the action");
  // The border of the maze blocks the move upwards
  require(!engine.submitAction(moveAction(Direction::UP)), "A move outside the maze must be rejected");
  require(engine.currentPlayer() == PLAYER_1, "A move outside the maze must keep the turn");
}

// Verifies the alternation of players and the extra actions of a turn
void testTurnOrder() {
  GameEngine engine(makeTestState());
  require(engine.submitAction(moveAction(Direction::RIGHT)), "A move through an open passage must be accepted");
  require(engine.state().players[PLAYER_1].position == Position{0, 1}, "Player 1 must move to the right");
  require(engine.state().players[PLAYER_2].position == Position{5, 5}, "Player 2 must not move");
  require(engine.currentPlayer() == PLAYER_2, "The turn must pass to player 2");
  require(engine.state().turnCount == 1, "The turn counter must increase");

  // A player with two actions plays twice before the turn changes
  GameState twoActions = makeTestState();
  twoActions.actionsLeft = 2;
  GameEngine second(std::move(twoActions));
  second.submitAction(moveAction(Direction::RIGHT));
  require(second.currentPlayer() == PLAYER_1, "The same player must act again with two actions");
  second.submitAction(moveAction(Direction::LEFT));
  require(second.currentPlayer() == PLAYER_2, "The turn must pass after the second action");

  // A blocked player loses exactly one turn
  GameState skipped = makeTestState();
  skipped.players[PLAYER_2].skipNextTurn = true;
  GameEngine third(std::move(skipped));
  third.submitAction(moveAction(Direction::RIGHT));
  require(third.currentPlayer() == PLAYER_1, "A blocked player must lose exactly one turn");
  require(!third.state().players[PLAYER_2].skipNextTurn, "The skip mark must be consumed once");
}

// Verifies the Jump Wall rules: inventory, bounds and the wall staying closed
void testJumpWall() {
  GameEngine noJumps(makeTestState());
  require(!noJumps.submitAction(jumpAction(Direction::DOWN)), "A jump without Jump Wall must be rejected");
  require(noJumps.currentPlayer() == PLAYER_1, "A rejected jump must keep the turn");

  GameState state = makeTestState();
  state.players[PLAYER_1].jumpWalls = 1;
  GameEngine engine(std::move(state));
  // A jump cannot leave the maze
  require(!engine.submitAction(jumpAction(Direction::UP)), "A jump outside the maze must be rejected");
  require(engine.state().players[PLAYER_1].jumpWalls == 1, "A rejected jump must keep the Jump Wall");

  require(engine.submitAction(jumpAction(Direction::DOWN)), "A jump through a wall must be accepted");
  require(engine.state().players[PLAYER_1].position == Position{1, 0}, "The player must cross the wall");
  require(engine.state().players[PLAYER_1].jumpWalls == 0, "The Jump Wall must be spent");
  require(!engine.state().maze.hasPassage(Position{0, 0}, Position{1, 0}), "The wall must stay closed");
  require(engine.currentPlayer() == PLAYER_2, "The turn must pass after the jump");
}

// Verifies that Double Play grants an extra action in the current turn
void testDoublePlay() {
  GameState state = makeTestState();
  state.maze.cellAt(Position{0, 1}).powerUp = PowerUpType::DOUBLE_PLAY;
  GameEngine engine(std::move(state));
  engine.submitAction(moveAction(Direction::RIGHT));
  // The extra action is granted before the move is charged
  require(engine.currentPlayer() == PLAYER_1, "Double Play must keep the turn of the collector");
  require(engine.state().actionsLeft == ACTIONS_PER_TURN, "One action must be left after the move");
  require(engine.state().maze.cellAt(Position{0, 1}).powerUp == PowerUpType::NONE, "The power-up must be removed");
  require(engine.state().stats.powerUpsCollected == 1, "The collected power-up must be counted");
}

// Verifies Control Enemy and the protection given by a shield
void testControlEnemy() {
  GameState state = makeTestState();
  state.maze.cellAt(Position{0, 1}).powerUp = PowerUpType::CONTROL_ENEMY;
  GameEngine engine(std::move(state));
  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.currentPlayer() == PLAYER_1, "The rival must lose exactly one turn");
  require(!engine.state().players[PLAYER_2].skipNextTurn, "The skip mark must be already consumed");

  // A shield cancels the effect and is spent
  GameState shielded = makeTestState();
  shielded.maze.cellAt(Position{0, 1}).powerUp = PowerUpType::CONTROL_ENEMY;
  shielded.players[PLAYER_2].shieldCount = 1;
  GameEngine second(std::move(shielded));
  second.submitAction(moveAction(Direction::RIGHT));
  require(second.currentPlayer() == PLAYER_2, "A shield must protect the turn of the rival");
  require(second.state().players[PLAYER_2].shieldCount == 0, "The shield must be spent");
}

// Verifies that entering mud costs MUD_COST actions in total
void testMudPenalty() {
  GameState state = makeTestState();
  state.maze.cellAt(Position{0, 1}).terrain = TerrainType::MUD;
  state.maze.openPassage(Position{5, 5}, Direction::RIGHT);
  GameEngine engine(std::move(state));
  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.state().players[PLAYER_1].pendingMudActions == MUD_COST - 1, "Mud must be charged once per move");
  require(engine.currentPlayer() == PLAYER_2, "The move must use the whole turn");
  // The debt is paid when the turn comes back
  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.state().players[PLAYER_1].pendingMudActions == 0, "The debt must be paid on the next turn");
  require(engine.currentPlayer() == PLAYER_2, "Paying the debt must cost player 1 a whole turn");

  // With an extra action the debt is paid right away
  GameState withExtra = makeTestState();
  withExtra.maze.cellAt(Position{0, 1}).terrain = TerrainType::MUD;
  withExtra.actionsLeft = 2;
  GameEngine second(std::move(withExtra));
  second.submitAction(moveAction(Direction::RIGHT));
  require(second.state().players[PLAYER_1].pendingMudActions == 0, "An extra action must pay the debt right away");
  require(second.currentPlayer() == PLAYER_2, "The move and the mud must use both actions");
}

// Verifies that a portal teleports once and then disappears
void testPortalOneUse() {
  GameState state = makeTestState();
  state.maze.cellAt(Position{0, 1}).portalID = 1;
  state.maze.cellAt(Position{3, 3}).portalID = 1;
  GameEngine engine(std::move(state));

  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.state().players[PLAYER_1].position == Position{3, 3}, "The player must be teleported to the pair");
  require(engine.state().maze.cellAt(Position{0, 1}).portalID == PORTAL_ID, "The origin portal must disappear");
  require(engine.state().maze.cellAt(Position{3, 3}).portalID == PORTAL_ID, "The destination portal must disappear");
  require(engine.currentPlayer() == PLAYER_2, "Teleporting must not cost an extra action");

  // Walking over the used cell again must not teleport

  require(engine.state().maze.cellAt(Position{0, 1}).portalID == PORTAL_ID, "The origin portal must stay removed");
    // The pair is gone, so neither cell can teleport anyone again
  require(engine.state().players[PLAYER_1].position == Position{3, 3}, "The player must stay where the portal left it");
}

// Verifies that a shield tile gives one shield and is removed
void testShieldTile() {
  GameState state = makeTestState();
  state.maze.cellAt(Position{0, 1}).special = SpecialCellType::SHIELD_TILE;
  GameEngine engine(std::move(state));
  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.state().players[PLAYER_1].shieldCount == 1, "The shield tile must give one shield");
  require(engine.state().maze.cellAt(Position{0, 1}).special == SpecialCellType::NONE, "The shield tile must be removed");
  require(engine.state().players[PLAYER_2].shieldCount == 0, "The rival must get nothing");
}

// Verifies the Fog of War radius, which depends on distance and not on walls
void testFogRadius() {
  Position viewer{0, 0};
  require(isCellVisible(viewer, Position{2, 0}, 2), "With radius 2 the cell at distance 2 must be visible");
  require(!isCellVisible(viewer, Position{2, 1}, 2), "With radius 2 the cell at distance 3 must be hidden");
  require(isCellVisible(viewer, viewer, 2), "The cell of the viewer must be visible");
  require(isCellVisible(viewer, Position{0, FOG_RADIUS}), "The default radius must be FOG_RADIUS");
  require(!isCellVisible(viewer, Position{0, FOG_RADIUS + 1}), "A cell past FOG_RADIUS must be hidden");

  GameState state = makeTestState();
  state.players[PLAYER_1].position = Position{5, 10};
  GameEngine engine(std::move(state));
  std::vector<Position> cells = engine.visibleCells(PLAYER_1);
  require(contains(cells, Position{5, 10}), "The player must see its own cell");
  require(contains(cells, Position{5 - FOG_RADIUS, 10}), "The cell exactly on the radius must be listed");
  require(!contains(cells, Position{5 - FOG_RADIUS - 1, 10}), "The cell past the radius must not be listed");

  // No listed cell may fall outside the radius
  for (const Position& cell : cells) {
    require(manhattanDistance(Position{5, 10}, cell) <= FOG_RADIUS, "A listed cell is outside the radius");
  }
}

// Verifies that reaching the treasure ends the match
void testWin() {
  GameState state = makeTestState();
  state.treasure = Position{0, 1};
  GameEngine engine(std::move(state));
  engine.submitAction(moveAction(Direction::RIGHT));
  require(engine.isGameOver(), "Reaching the treasure must end the match");
  require(engine.state().winner == PLAYER_1, "The player that arrives must win");
  require(!engine.submitAction(moveAction(Direction::LEFT)), "No action must be accepted after winning");

  // The victory is also detected at the end of a teleport
  GameState viaPortal = makeTestState();
  viaPortal.maze.cellAt(Position{0, 1}).portalID = 1;
  viaPortal.maze.cellAt(Position{3, 3}).portalID = 1;
  viaPortal.treasure = Position{3, 3};
  GameEngine second(std::move(viaPortal));
  second.submitAction(moveAction(Direction::RIGHT));
  require(second.state().winner == PLAYER_1, "A portal that lands on the treasure must win");
}

// Builds a small match with every wall closed except the passage (0,0)-(0,1).
// Player 1 starts at (0,0), player 2 at (5,5) and the treasure is outside the
// maze, so no move wins by accident.
GameState makeTestState() {
  GameState state;
  state.players[PLAYER_1].position = Position{0, 0};
  state.players[PLAYER_2].position = Position{5, 5};
  state.maze.openPassage(Position{0, 0}, Direction::RIGHT);
  return state;
}

// Builds a move action in a direction
Action moveAction(Direction direction) {
  return Action{ActionType::MOVE, direction};
}

// Builds a Jump Wall action in a direction
Action jumpAction(Direction direction) {
  return Action{ActionType::JUMP_WALL, direction};
}

// Tells whether a list of cells holds a given cell
bool contains(const std::vector<Position>& cells, Position target) {
  for (const Position& cell : cells) {
    if (cell == target) {
      return true;
    }
  }
  return false;
}