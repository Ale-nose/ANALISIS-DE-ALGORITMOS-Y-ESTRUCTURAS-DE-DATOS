// Copyright Alejandro, Ashley y Kevin
#include "GameLogger.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ostream>
#include <system_error>

#include "GameConfig.hpp"

namespace {

inline constexpr const char* LOG_FOLDER = "logs";
inline constexpr const char* LOG_PREFIX = "mazecraze_";
inline constexpr const char* LOG_EXTENSION = ".txt";
inline constexpr int LOG_DECIMALS = 3;

const char* modeName(GameMode mode) {
  switch (mode) {
    case GameMode::SINGLE_PLAYER:
      return "Single player (vs AI)";
    case GameMode::LOCAL_TWO_PLAYERS:
      return "Local two players";
    case GameMode::ONLINE:
      return "Online";
  }
  return "Unknown";
}

const char* generatorName(GenerationAlgorithm generator) {
  switch (generator) {
    case GenerationAlgorithm::DFS:
      return "DFS";
    case GenerationAlgorithm::BFS:
      return "BFS";
  }
  return "Unknown";
}

const char* searchName(SearchAlgorithm algorithm) {
  switch (algorithm) {
    case SearchAlgorithm::BFS:
      return "BFS";
    case SearchAlgorithm::DIJKSTRA:
      return "Dijkstra";
    case SearchAlgorithm::A_STAR:
      return "A*";
  }
  return "Unknown";
}

const char* reasonName(ReplanReason reason) {
  switch (reason) {
    case ReplanReason::START:
      return "START (first path)";
    case ReplanReason::PORTAL:
      return "PORTAL (teleported off the path)";
    case ReplanReason::POWER_UP:
      return "POWER_UP";
    case ReplanReason::BLOCKED_ROUTE:
      return "BLOCKED_ROUTE (left the path)";
  }
  return "Unknown";
}

std::string playerName(const GameState& state, int player) {
  if (player == PLAYER_1) {
    return "Player 1";
  }
  if (state.options.mode == GameMode::SINGLE_PLAYER) {
    return "Player 2 (AI)";
  }
  return "Player 2";
}

void writeTitle(std::ostream& out, const char* title) {
  out << "\n=== " << title << " ===\n";
}

void writeConfiguration(std::ostream& out, const GameOptions& options) {
  writeTitle(out, "CONFIGURATION");
  out << "Mode: " << modeName(options.mode) << '\n'
      << "Generator: " << generatorName(options.generator) << '\n'
      << "Fog of War: " << (options.fogEnabled ? "on" : "off") << '\n'
      << "Seed: " << options.seed << '\n'
      << "Maze size: " << options.rows << " rows x " << options.cols
      << " cols\n"
      << "Extra edge probability: " << options.extraEdgeProb << '\n'
      << "Mud spawn rate: " << options.mudSpawnRate << '\n';
}

void writeConstants(std::ostream& out) {
  writeTitle(out, "CONSTANTS");
  out << "NORMAL_COST: " << NORMAL_COST << '\n'
      << "MUD_COST: " << MUD_COST << '\n'
      << "FOG_RADIUS: " << FOG_RADIUS << '\n'
      << "ACTIONS_PER_TURN: " << ACTIONS_PER_TURN << '\n'
      << "POWER_SPAWN_RATE: " << POWER_SPAWN_RATE << '\n'
      << "PORTAL_SPAWN_RATE: " << PORTAL_SPAWN_RATE << '\n'
      << "SHIELD_SPAWN_RATE: " << SHIELD_SPAWN_RATE << '\n'
      << "JUMP_WALL_PROB (AI): " << JUMP_WALL_PROB << '\n';
}

void writeGeneration(std::ostream& out, const GameState& state) {
  const GenerationMetrics& metrics = state.stats.generation;
  writeTitle(out, "MAZE GENERATION");
  out << "Algorithm: " << generatorName(state.options.generator) << '\n'
      << "Visited nodes: " << metrics.visitedNodes << '\n'
      << "Created edges: " << metrics.createdEdges << '\n'
      << "Extra edges: " << metrics.extraEdges << '\n'
      << "CPU time (ms): " << metrics.cpuTimeMs << '\n';
}

void writeSearches(std::ostream& out, const GameStats& stats) {
  writeTitle(out, "PATH SEARCHES");
  if (stats.searches.empty()) {
    out << "No searches were recorded.\n";
    return;
  }

  int number = 1;
  for (const SearchRecord& record : stats.searches) {
    out << '#' << number++ << ' ' << searchName(record.algorithm);
    // Dijkstra runs once at setup, the reason only matters for the AI's A*
    if (record.algorithm != SearchAlgorithm::DIJKSTRA) {
      out << " | reason: " << reasonName(record.reason);
    } else {
      out << " | setup: path between the players (places the treasure)";
    }
    out << '\n'
        << "    visited nodes: " << record.metrics.visitedNodes
        << " | path cost: " << record.metrics.pathCost
        << " | CPU time (ms): " << record.metrics.cpuTimeMs << '\n';
  }
}

void writeReplans(std::ostream& out, const GameStats& stats) {
  writeTitle(out, "AI REPLANS");
  out << "Total: " << stats.aiReplans.size() << '\n';
  int number = 1;
  for (ReplanReason reason : stats.aiReplans) {
    out << '#' << number++ << ' ' << reasonName(reason) << '\n';
  }
}

void writeSummary(std::ostream& out, const GameState& state) {
  const GameStats& stats = state.stats;
  writeTitle(out, "FINAL SUMMARY");
  out << "Turns played: " << stats.turnsPlayed << '\n'
      << "Power-ups collected: " << stats.powerUpsCollected << '\n'
      << "Power-ups used: " << stats.powerUpsUsed << '\n'
      << "Portals used: " << stats.portalsUsed << '\n';

  for (int player = 0; player < NUM_PLAYERS; ++player) {
    const PlayerState& data = state.players[static_cast<std::size_t>(player)];
    out << playerName(state, player) << " ended at (" << data.position.row
        << ", " << data.position.col << ") with " << data.jumpWalls
        << " Jump Wall(s) and " << data.shieldCount << " shield(s)\n";
  }

  out << "Winner: "
      << (state.winner == PLAYER_NONE ? std::string("none")
        : playerName(state, state.winner))
      << '\n';
}

}  // namespace

std::string GameLogger::makeLogFilename() {
  const std::time_t now =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
  std::tm localTime{};
#ifdef _WIN32
  localtime_s(&localTime, &now);
#else
  localtime_r(&now, &localTime);
#endif

  char stamp[32];
  std::strftime(stamp, sizeof(stamp), "%Y-%m-%d_%H-%M-%S", &localTime);

  const std::string base =
      std::string(LOG_FOLDER) + "/" + LOG_PREFIX + stamp;
  std::string name = base + LOG_EXTENSION;

  // Two matches in the same second: add a number instead of overwriting
  for (int copy = 2; std::filesystem::exists(name); ++copy) {
    name = base + "_" + std::to_string(copy) + LOG_EXTENSION;
  }
  return name;
}

bool GameLogger::writeLog(const GameState& state
    , const std::string& filename) {
  const std::filesystem::path path(filename);
  if (path.has_parent_path()) {
    std::error_code error;
    std::filesystem::create_directories(path.parent_path(), error);
    if (error) {
      return false;
    }
  }

  std::ofstream out(path);
  if (!out.is_open()) {
    return false;
  }
  out << std::fixed << std::setprecision(LOG_DECIMALS);

  out << "MAZE CRAZE - GAME LOG\n";
  writeConfiguration(out, state.options);
  writeConstants(out);
  writeGeneration(out, state);
  writeSearches(out, state.stats);
  writeReplans(out, state.stats);
  writeSummary(out, state);

  out.flush();
  return out.good();  // false if any write failed (disk full, etc.)
}
