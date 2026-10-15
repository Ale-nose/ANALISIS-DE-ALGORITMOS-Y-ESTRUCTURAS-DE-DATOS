// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <vector>
#include "Metrics.hpp"

enum class ReplanReason {
  START,
  PORTAL,
  POWER_UP,
  BLOCKED_ROUTE
};

struct SearchRecord {
  SearchAlgorithm algorithm = SearchAlgorithm::A_STAR;
  ReplanReason reason = ReplanReason::START;
  SearchMetrics metrics;

};

struct GameStats {
  GenerationMetrics generation;
  std::vector<SearchRecord> searches;
  std::vector<ReplanReason> aiReplans;
  int turnsPlayed = 0;
  int powerUpsCollected = 0;
  int powerUpsUsed = 0;
  int portalsUsed = 0;
};
