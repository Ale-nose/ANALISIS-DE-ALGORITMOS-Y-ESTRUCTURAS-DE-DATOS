// Copyright Alejandro, Ashley y Kevin
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

// test_algorithm_metrics.cpp, test_bfs.cpp, test_dfs.cpp,
// test_bfs_pathfinding.cpp, test_start_positions.cpp (Kevin)
void testAlgorithmMetrics();
void testBfsGeneration();
void testDfsGeneration();
void testBfsPathfinding();
void testStartPositions();

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

// test_ai.cpp (Alejandro)
void testAiFallbackJump();
void testAiFallbackStep();
void testAiNoOscillation();
void testAiGreedyJump();
void testAiKeepsJump();
void testAiPowerUpDetour();
void testAiPowerUpRejected();
void testAiRandomPolicy();

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

  failures += run(testAlgorithmMetrics, "Algorithm metrics") ? 0 : 1;
  failures += run(testBfsGeneration, "BFS generation") ? 0 : 1;
  failures += run(testDfsGeneration, "DFS generation") ? 0 : 1;
  failures += run(testBfsPathfinding, "BFS pathfinding") ? 0 : 1;
  failures += run(testStartPositions, "Start positions") ? 0 : 1;

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

  failures += run(testAiFallbackJump, "AI fallback jump") ? 0 : 1;
  failures += run(testAiFallbackStep, "AI fallback step") ? 0 : 1;
  failures += run(testAiNoOscillation, "AI no oscillation") ? 0 : 1;
  failures += run(testAiGreedyJump, "AI greedy jump") ? 0 : 1;
  failures += run(testAiKeepsJump, "AI keeps jump") ? 0 : 1;
  failures += run(testAiPowerUpDetour, "AI power-up detour") ? 0 : 1;
  failures += run(testAiPowerUpRejected, "AI power-up rejected") ? 0 : 1;
  failures += run(testAiRandomPolicy, "AI random policy") ? 0 : 1;

  std::cout << failures << " failure(s)\n";
  return failures == 0 ? 0 : 1;
}
