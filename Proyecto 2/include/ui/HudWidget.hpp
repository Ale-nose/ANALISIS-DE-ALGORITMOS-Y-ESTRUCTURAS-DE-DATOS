// Copyright Alejandro, Ashley y Kevin
#pragma once
 
#include <QWidget>
 
#include "GameState.hpp"
 
class QLabel;

/**
 * @brief Side panel with the data of the player whose turn it is: name,
 * actions left, Jump Walls, shields and warnings (mud or opponent losing a
 * turn). Only reads the GameState but never changes it.
 */
class HudWidget : public QWidget {
 public:
  /**
   * @brief Builds the panel labels and applies the HUD style.
   * @param parent Optional parent widget.
   */
  explicit HudWidget(QWidget* parent = nullptr);

  /**
   * @brief Shows the data of the current player taken from the state.
   * @param state Current state of the match, as kept by the GameEngine.
   */
  void updateFromState(const GameState& state);

  /**
   * @brief Hides or shows the private inventory: Jump Walls and shields are
   * shown as "?". It is used in local Fog of War so the next player's items
   * are not revealed before the turn screen is confirmed. The change is
   * visible on the next updateFromState call.
   * @param hidden True to hide the inventory, false to show it.
   */
  void setPrivateInfoHidden(bool hidden);

 private:
  QLabel* playerLabel_;
  QLabel* actionsLabel_;
  QLabel* jumpWallsLabel_;
  QLabel* shieldsLabel_;
  QLabel* noticeLabel_;
  bool privateInfoHidden_ = false;
};
