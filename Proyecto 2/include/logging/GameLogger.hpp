// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <string>

#include "GameState.hpp"

/**
 * @brief Writes the full report of a finished match to a text file.
 * @note It only reads the GameState. It does not depend on Qt.
 */
class GameLogger {
 public:
  /**
   * @brief Builds a file name that is unique for every match:
   * logs/mazecraze_YYYY-MM-DD_HH-MM-SS.txt.
   * @return Relative path of the new log file.
   * @note If it already exists, a number is added (_2, _3...) so a previous
   * log is never overwritten.
   */
  static std::string makeLogFilename();

  /**
   * @brief Writes the log of a match: configuration, constants, generation
   * metrics, every search, AI replans and the final summary.
   * @param state State of the match (normally the finished one).
   * @param filename Path of the file to create. Missing folders are created.
   * @return True if the whole file was written; false if it could not be
   * opened or saved.
   */
  static bool writeLog(const GameState& state, const std::string& filename);
};
