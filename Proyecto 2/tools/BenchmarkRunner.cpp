// Copyright Alejandro, Ashley y Kevin
#include <array>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "GameConfig.hpp"
#include "GameEngine.hpp"
#include "GameSetup.hpp"
#include "MatchSimulator.hpp"

// 2 rounds of 18 combinations (the report needs 30)
constexpr int BENCHMARK_MATCHES = 36;
constexpr unsigned int BENCHMARK_BASE_SEED = 2026u;  ///< Seed of the first match
constexpr int BENCHMARK_COLS_PER_ROW = 2;            ///< Columns = rows * this value
constexpr std::array<int, 3> BENCHMARK_ROWS{10, 15, 20};                ///< Map sizes
constexpr std::array<double, 3> BENCHMARK_MUD_RATES{0.05, 0.12, 0.25};  ///< Mud rates
constexpr const char* RESULTS_FILE = "data/benchmark_results.csv";  ///< One row per match
constexpr const char* SEEDS_FILE = "data/seeds.txt";                ///< Seeds used

/**
 * @brief One row of data/benchmark_results.csv: a complete match.
 */
struct BenchmarkRow {
  unsigned int seed;
  int rows, cols, visited, pathCost, turns, winner;
  GenerationAlgorithm generator;
  double mudRate, cpuMs;
};

/**
 * @brief Plays one complete match and summarizes it.
 * @param options Settings of the match.
 * @return The row of results.
 * @throws std::runtime_error If the map has no path between the players.
 */
BenchmarkRow runAutomatedMatch(const GameOptions& options) {
  GameEngine engine(GameSetup::createGame(options));
  std::mt19937 rng(options.seed);
  const bool finished = playAutomatedMatch(engine, rng);

  const GameState& state = engine.state();
  BenchmarkRow row{};
  row.seed = options.seed;
  row.rows = options.rows;
  row.cols = options.cols;
  row.generator = options.generator;
  row.mudRate = options.mudSpawnRate;
  row.visited = state.stats.generation.visitedNodes;
  row.cpuMs = state.stats.generation.cpuTimeMs;
  row.pathCost = state.stats.searches.front().metrics.pathCost;  // setup Dijkstra
  row.turns = state.stats.turnsPlayed;
  row.winner = finished ? state.winner : PLAYER_NONE;
  return row;
}

/**
 * @brief Plays matchCount complete matches. Consecutive matches share the
 * seed and the size but use DFS and BFS, so both generators are compared on
 * equal terms.
 * @param matchCount Number of matches.
 * @return One row per match.
 */
std::vector<BenchmarkRow> runBenchmarks(int matchCount) {
  const std::size_t sizes = BENCHMARK_ROWS.size();
  const std::size_t rates = BENCHMARK_MUD_RATES.size();
  std::vector<BenchmarkRow> rows;

  for (int i = 0; i < matchCount; ++i) {
    const std::size_t combination = static_cast<std::size_t>(i / 2);  // DFS and BFS share it
    GameOptions options;
    options.generator = i % 2 == 0 ? GenerationAlgorithm::DFS
      : GenerationAlgorithm::BFS;
    options.rows = BENCHMARK_ROWS[(combination / rates) % sizes];
    options.cols = options.rows * BENCHMARK_COLS_PER_ROW;
    options.mudSpawnRate = BENCHMARK_MUD_RATES[combination % rates];
    options.seed = BENCHMARK_BASE_SEED + static_cast<unsigned int>(i / 2);

    // A seed can give a map without path between the players, try the next
    for (int attempt = 0; attempt < 10; ++attempt) {
      try {
        rows.push_back(runAutomatedMatch(options));
        break;
      } catch (const std::runtime_error&) {
        ++options.seed;
      }
    }
  }
  return rows;
}

/**
 * @brief Saves the matches as CSV.
 * @param filename Path of the file (its folder is created if needed).
 * @param rows Rows to save.
 * @return True if the file was written.
 */
bool saveBenchmarkCsv(const std::string& filename, const std::vector<BenchmarkRow>& rows) {
  std::filesystem::create_directories(std::filesystem::path(filename).parent_path());
  std::ofstream csv(filename);
  if (!csv.is_open()) {
    return false;
  }

  csv << std::fixed << std::setprecision(6);
  csv << "seed,rows,cols,generator,mudRate,visited,cpuMs,pathCost,turns,winner\n";
  for (const BenchmarkRow& row : rows) {
    csv << row.seed << ',' << row.rows << ',' << row.cols << ','
      << (row.generator == GenerationAlgorithm::DFS ? "DFS" : "BFS") << ','
      << row.mudRate << ',' << row.visited << ',' << row.cpuMs << ','
      << row.pathCost << ',' << row.turns << ',' << row.winner << '\n';
  }
  return csv.good();
}

/**
 * @brief Saves the seeds used, one per line, so the run can be repeated.
 * @param filename Path of the file.
 * @param rows Matches played.
 * @return True if the file was written.
 */
bool saveSeeds(const std::string& filename, const std::vector<BenchmarkRow>& rows) {
  std::ofstream out(filename);
  for (const BenchmarkRow& row : rows) {
    out << row.seed << '\n';
  }
  return out.good();
}

/**
 * @brief Plays the benchmark matches and saves the CSV and the seeds.
 * @return 0 if every match ended and the files were written, 1 otherwise.
 */
int main() {
  const std::vector<BenchmarkRow> rows = runBenchmarks(BENCHMARK_MATCHES);

  int finished = 0;
  for (const BenchmarkRow& row : rows) {
    finished += row.winner != PLAYER_NONE ? 1 : 0;
  }
  std::cout << "Finished matches: " << finished << " / " << rows.size() << '\n';

  if (!saveBenchmarkCsv(RESULTS_FILE, rows) || !saveSeeds(SEEDS_FILE, rows)) {
    std::cerr << "Error: could not write the files in data/\n";
    return 1;
  }
  return finished == static_cast<int>(rows.size()) ? 0 : 1;
}
