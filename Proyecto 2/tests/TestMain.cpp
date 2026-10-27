// Copyright Alejandro, Ashley y Kevin
#include <exception>
#include <iostream>

// Declarations of test functions defined in CoreTests.cpp
void testMazeWalls();
void testDfsConnectivity();
void testBfsConnectivity();
void testExtraPassages();
void testDijkstraMud();
void testBfsSteps();
void testTreasureMidpoint();

// Declarations of test functions defined in individual test_*.cpp files
void testAlgorithmMetrics();
void testBfsPathfinding();
void testBfsGeneration();
void testDfsGeneration();
void testStartPositions();

/**
 * @brief Main entry point for the independent test suite executable (MazeCrazeTests).
 * @return Exit status 0 on successful test suite execution, 1 on failure.
 */
int main() {
  try {
    // Execute core test suite
    testMazeWalls();
    testDfsConnectivity();
    testBfsConnectivity();
    testExtraPassages();
    testDijkstraMud();
    testBfsSteps();
    testTreasureMidpoint();

    // Execute algorithm metrics, pathfinding, generation, and start position tests
    testAlgorithmMetrics();
    testBfsPathfinding();
    testBfsGeneration();
    testDfsGeneration();
    testStartPositions();

    std::cout << "[PASS] All MazeCrazeTests passed successfully!\n";
    return 0;
  } catch (const std::exception& error) {
    std::cerr << "[FAIL] Test suite failed with exception: " << error.what() << '\n';
    return 1;
  }
}
