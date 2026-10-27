/*
Entry point of the test executable.
Each test file declares its tests here and calls them from main as they are added.
*/
#include <iostream>
#include <stdexcept>
#include <string>

// CoreTests.cpp (Kevin)
void testMazeWalls();
void testDfsConnectivity();
void testBfsConnectivity();
void testExtraPassages();
void testDijkstraMud();
void testBfsSteps();
void testTreasureMidpoint();

// GameTests.cpp (Alejandro)
void testBlockedMove();
void testTurnOrder();
void testJumpWall();
void testDoublePlay();
void testControlEnemy();
void testMudPenalty();
void testPortalOneUse();
void testShieldTile();
void testFogRadius();
void testWin();

// Runs one test and reports whether it passed
bool run(void (*test)(), const std::string& name) {
  try {
    test();
    std::cout << "[PASS] " << name << '\n';
    return true;
  } catch (const std::exception& error) {
    std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
    return false;
  }
}

int main() {
  int failures = 0;

  failures += run(testMazeWalls, "Maze walls") ? 0 : 1;
  failures += run(testDfsConnectivity, "DFS connectivity") ? 0 : 1;
  failures += run(testBfsConnectivity, "BFS connectivity") ? 0 : 1;
  failures += run(testExtraPassages, "Extra passages") ? 0 : 1;
  failures += run(testDijkstraMud, "Dijkstra with mud") ? 0 : 1;
  failures += run(testBfsSteps, "BFS steps") ? 0 : 1;
  failures += run(testTreasureMidpoint, "Treasure midpoint") ? 0 : 1;

  failures += run(testBlockedMove, "Blocked move") ? 0 : 1;
  failures += run(testTurnOrder, "Turn order") ? 0 : 1;
  failures += run(testJumpWall, "Jump Wall") ? 0 : 1;
  failures += run(testDoublePlay, "Double Play") ? 0 : 1;
  failures += run(testControlEnemy, "Control Enemy") ? 0 : 1;
  failures += run(testMudPenalty, "Mud penalty") ? 0 : 1;
  failures += run(testPortalOneUse, "Portal one use") ? 0 : 1;
  failures += run(testShieldTile, "Shield tile") ? 0 : 1;
  failures += run(testFogRadius, "Fog radius") ? 0 : 1;
  failures += run(testWin, "Win") ? 0 : 1;

  std::cout << failures << " failure(s)\n";
  return failures == 0 ? 0 : 1;
}