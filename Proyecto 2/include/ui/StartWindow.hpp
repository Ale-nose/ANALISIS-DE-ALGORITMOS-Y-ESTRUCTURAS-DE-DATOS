// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QWidget>

#include "GameState.hpp"

class QRadioButton;
class QCheckBox;

/**
 * @brief Start menu: lets the user pick game mode, maze generator and
 * Fog of War.
 * It never creates the match; it only reports the chosen GameOptions through
 * the gameStartRequested signal.
 */
class StartWindow : public QWidget {
  Q_OBJECT

 public:
  /**
   * @brief Builds the menu widgets and applies the application style.
   * @param parent Optional parent widget (the window owns its children).
   */
  explicit StartWindow(QWidget* parent = nullptr);

  /**
   * @brief Reads the options currently selected in the menu.
   * Rows, columns and spawn rates keep their GameConfig defaults. The seed is
   * random so every match generates a different maze.
   * @return The GameOptions that describe the selected match.
   */
  GameOptions selectedOptions() const;

 signals:
  /**
   * @brief Emitted when the user presses START.
   * @param options The options selected in the menu.
   */
  void gameStartRequested(const GameOptions& options);

 private slots:
  /**
   * @brief Reads the selected options and emits gameStartRequested.
   */
  void startGame();

 private:
  QRadioButton* singlePlayerButton_;
  QRadioButton* localTwoPlayersButton_;
  QRadioButton* dfsButton_;
  QRadioButton* bfsButton_;
  QCheckBox* fogCheckBox_;
};
