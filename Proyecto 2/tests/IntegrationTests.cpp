// Copyright Alejandro, Ashley y Kevin
#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

#include "GameConfig.hpp"
#include "GameEngine.hpp"
#include "GameLogger.hpp"
#include "GameSetup.hpp"
#include "MatchSimulator.hpp"

/**
 * @brief Integration tests of the whole game without the window: create a match,
 * play it until the end and write its game log.
 * Usage: IntegrationTests (exit code 0 if everything passes)
 */

namespace {

int failures = 0;  // number of checks that failed so far

/**
 * @brief Prints the result of one check and counts it if it fails.
 * @param condition True if the check passed.
 * @param description What was checked.
 */
void check(bool condition, const std::string& description) {
  std::cout << (condition ? "  [PASS] " : "  [FAIL] ") << description << '\n';

  if (!condition) {
    ++failures;
  }
}

/**
 * @brief Builds the options of a test match.
 * @param mode Game mode.
 * @param generator Maze generation algorithm.
 * @param fog True to enable Fog of War.
 * @param seed Seed of the match.
 * @return Options with the default size and rates.
 */
GameOptions makeOptions(GameMode mode, GenerationAlgorithm generator
    , bool fog, unsigned int seed) {
  GameOptions options;
  options.mode = mode;
  options.generator = generator;
  options.fogEnabled = fog;
  options.seed = seed;
  return options;
}

/**
 * @brief Short description of a match, used in the check messages.
 * @param options Options of the match.
 * @return Text such as "single/DFS/no fog seed 101".
 */
std::string label(const GameOptions& options) {
  std::ostringstream text;
  text << (options.mode == GameMode::SINGLE_PLAYER ? "single" : "local")
    << '/' << (options.generator == GenerationAlgorithm::DFS ? "DFS" : "BFS")
    << (options.fogEnabled ? "/fog" : "/no fog") << " seed "
    << options.seed;

  return text.str();
}

/**
 * @brief Reads a whole text file.
 * @param filename Path of the file.
 * @return Its content, empty if it cannot be opened.
 */
std::string readFile(const std::string& filename) {
  std::ifstream in(filename);
  std::ostringstream text;
  text << in.rdbuf();
  return text.str();
}

/**
 * @brief Tells whether a text contains another one.
 * @param text Text to search in.
 * @param part Text to search for.
 * @return True if part is inside text.
 */
bool contains(const std::string& text, const std::string& part) {
  return text.find(part) != std::string::npos;
}

// Game modes that are tested
const std::array<GameMode, 2> MODES{GameMode::SINGLE_PLAYER
  , GameMode::LOCAL_TWO_PLAYERS};

// Generation algorithms that are tested
const std::array<GenerationAlgorithm, 2> GENERATORS{GenerationAlgorithm::DFS
  , GenerationAlgorithm::BFS};

}  // namespace

/**
 * @brief Every menu combination (mode, generator and fog) creates a valid,
 * ready-to-play match.
 */
void testCreateMatch() {
  std::cout << "testCreateMatch\n";
  unsigned int seed = 100;

  for (GameMode mode : MODES) {
    for (GenerationAlgorithm generator : GENERATORS) {
      for (bool fog : {false, true}) {
        const GameOptions options = makeOptions(mode, generator, fog, ++seed);
        const std::string name = label(options);

        try {
          GameEngine engine(GameSetup::createGame(options));
          const GameState& state = engine.state();
          const Position start1 = state.players[PLAYER_1].position;
          const Position start2 = state.players[PLAYER_2].position;

          check(state.maze.rows() == options.rows && state.maze.cols() 
            == options.cols, name + ": maze has the requested size");
          check(state.maze.isInside(start1) && state.maze.isInside(start2) &&
            !(start1 == start2), name 
            + ": both players start inside, in different cells");
          check(state.maze.isInside(state.treasure) &&
            !(state.treasure == start1) && !(state.treasure == start2)
            , name + ": treasure is inside and not on a start");
          check(state.options.mode == mode && state.options.generator == generator
            && state.options.fogEnabled == fog && state.options.seed == options.seed
            , name + ": the options are stored in the state");
          check(engine.currentPlayer() == PLAYER_1 && !engine.isGameOver()
            , name + ": Player 1 starts and the match is not over");
          check(state.stats.searches.size() == 1 
            && state.stats.searches.front().algorithm == SearchAlgorithm::DIJKSTRA
            , name + ": the setup Dijkstra search is recorded");

          bool seesItself = false;
          for (const Position& cell : engine.visibleCells(PLAYER_1)) {
            seesItself = seesItself || cell == start1;
          }
          check(seesItself, name + ": Fog view includes the player's cell");

        } catch (const std::exception& error) {
          check(false, name + ": createGame threw: " + error.what());
        }
      }
    }
  }
}

/**
 * @brief A complete match ends with a winner standing on the treasure, every
 * AI search is recorded once, and the same seed repeats the result.
 */
void testMatchFinishes() {
  std::cout << "testMatchFinishes\n";
  unsigned int seed = 200;

  for (GameMode mode : MODES) {
    for (GenerationAlgorithm generator : GENERATORS) {
      const GameOptions options = makeOptions(mode, generator, false, ++seed);
      const std::string name = label(options);

      int firstWinner = PLAYER_NONE;
      int firstTurns = -1;

      for (int run = 0; run < 2; ++run) {
        GameEngine engine(GameSetup::createGame(options));
        std::mt19937 rng(options.seed);
        const bool finished = playAutomatedMatch(engine, rng);
        const GameState& state = engine.state();

        if (run == 0) {
          check(finished && state.gameOver, name + ": the match finishes");
          check(state.winner == PLAYER_1 || state.winner == PLAYER_2
            , name + ": there is a winner");
          check(state.players[state.winner < 0 ? 0 : state.winner].position 
            == state.treasure, name + ": the winner stands on the treasure");
          check(state.stats.turnsPlayed > 0, name + ": turns were counted");
          check(!state.stats.aiReplans.empty() && state.stats.searches.size() 
            == 1 + state.stats.aiReplans.size(), name 
            + ": every AI search was recorded once");

          firstWinner = state.winner;
          firstTurns = state.stats.turnsPlayed;
        } else {
          check(state.winner == firstWinner && state.stats.turnsPlayed == firstTurns
            , name + ": the same seed repeats the result");
        }
      }
    }
  }
}

/**
 * @brief The game log is written with every section, its figures match the
 * statistics, a second log gets another name and writeLog returns false when
 * the file cannot be opened.
 */
void testGameLogWritten() {
  std::cout << "testGameLogWritten\n";
  const GameOptions options = makeOptions(GameMode::SINGLE_PLAYER
    , GenerationAlgorithm::DFS, false, 301);

  GameEngine engine(GameSetup::createGame(options));
  std::mt19937 rng(options.seed);
  check(playAutomatedMatch(engine, rng), "the match used for the log ends");
  const GameState& state = engine.state();

  const std::string first = GameLogger::makeLogFilename();
  check(GameLogger::writeLog(state, first), "writeLog returns true");

  const std::string text = readFile(first);
  for (const char* section : {"CONFIGURATION", "CONSTANTS", "MAZE GENERATION"
      , "PATH SEARCHES", "AI REPLANS", "FINAL SUMMARY", "Winner:"}) {
    check(contains(text, section), std::string("the log has ") + section);
  }

  check(contains(text, "Turns played: " + std::to_string(state.stats.turnsPlayed))
    , "the turns in the log match the statistics");
  check(contains(text, "Portals used: " + std::to_string(state.stats.portalsUsed))
    , "the portals in the log match the statistics");
  check(contains(text, "Seed: " + std::to_string(options.seed))
    , "the seed in the log matches the options");

  // A second match must not overwrite the first log
  const std::string second = GameLogger::makeLogFilename();
  check(second != first, "a second log gets a different file name");

  // The error case: the folder of the log is a regular file
  const std::filesystem::path blocker = std::filesystem::temp_directory_path()
    / "mazecraze_blocker.txt";
  std::ofstream(blocker) << "not a folder";
  check(!GameLogger::writeLog(state, (blocker / "log.txt").string())
    , "writeLog returns false when the file cannot be opened");

  // Clean up what the test created
  std::error_code ignored;
  std::filesystem::remove(blocker, ignored);
  std::filesystem::remove(first, ignored);
  std::filesystem::remove(second, ignored);
  std::filesystem::remove("logs", ignored);  // only if it is empty
}

/**
 * @brief Runs the three tests.
 * @return 0 if every check passed, 1 otherwise.
 */
int main() {
  testCreateMatch();
  testMatchFinishes();
  testGameLogWritten();

  std::cout << '\n' << (failures == 0 ? "ALL CHECKS PASSED" 
    : std::to_string(failures) + " CHECK(S) FAILED") << '\n';
  return failures == 0 ? 0 : 1;
}
