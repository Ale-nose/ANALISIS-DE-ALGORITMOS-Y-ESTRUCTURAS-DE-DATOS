// Copyright Alejandro, Ashley y Kevin

#pragma once

#include <QObject>
#include <QTimer>
#include <cstddef>

#include "Pathfinding.hpp"

class GameBoardWidget;

/**
 * @brief Plays a recorded A* search step by step on top of the board.
 *
 * It only draws: it receives a SearchTrace that was already computed and
 * reveals it one closed cell per timer tick, so the window never freezes.
 */
class AStarVisualizer : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Creates a visualizer that draws on the given board.
   * @param board Board that shows the overlay (not owned).
   * @param parent Qt parent object.
   */
  explicit AStarVisualizer(GameBoardWidget* board, QObject* parent = nullptr);

  /**
   * @brief Starts the animation of a recorded search.
   * @param trace Trace produced by findPathAStar(..., true).
   */
  void playTrace(const SearchTrace& trace);

  /** @brief Stops the animation and removes the overlay from the board. */
  void stopAnimation();

  /** @brief Returns true while the animation is still revealing cells. */
  bool isRunning() const;

 signals:
  /** @brief Emitted when the whole trace, including the final path, is shown.
   */
  void finished();

 private slots:
  void showNextStep();

 private:
  GameBoardWidget* board_;
  QTimer timer_;
  SearchTrace trace_;
  std::size_t shownClosed_ = 0;
};
