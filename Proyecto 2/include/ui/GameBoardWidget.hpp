// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QPoint>
#include <QRect>
#include <QSize>
#include <QWidget>

#include "GameState.hpp"

class QPainter;
class QPaintEvent;

/**
 * @brief Widget that draws the maze, the players, the treasure and every game
 * element (mud, portals, power-ups and Shield Tiles).
 * @note It only reads the GameState it was given. Call setGameState every 
 * time the state changes to trigger a repaint.
 */
class GameBoardWidget : public QWidget {
 public:
  /**
   * @brief Creates an empty board that expands to fill its container.
   * @param parent Optional parent widget.
   */
  explicit GameBoardWidget(QWidget* parent = nullptr);

  /**
   * @brief Stores a copy of the state to draw and repaints the board.
   * @param state Current state of the match.
   */
  void setGameState(const GameState& state);

  /**
   * @brief Comfortable size: BOARD_PREFERRED_CELL_SIZE per cell plus margins.
   * @return The preferred size of the board for UI layouts.
   */
  QSize sizeHint() const override;

  /**
   * @brief Smallest size that still keeps every cell readable.
   * @return The minimum size of the board to prevent UI overlapping.
   */
  QSize minimumSizeHint() const override;

  /**
   * @brief Largest whole cell size that lets every row and column fit inside
   * the given area without deforming the grid.
   * @param availableSize Area reserved for the grid (margins already removed).
   * @return Cell side in pixels, 0 if not even one pixel per cell fits.
   */
  int calculateCellSize(QSize availableSize) const;

  /**
   * @brief Size of the maze in cells.
   * @return A QSize where width is the number of columns and height the rows.
   */
  QSize gridSize() const;

 protected:
  /**
   * @brief Draws the whole board: background, cells, treasure and players.
   * @param event Paint event sent by Qt (unused: the board repaints fully).
   */
  void paintEvent(QPaintEvent* event) override;

 private:
  /**
   * @brief Computes cellSize_ and boardOrigin_ mathematically so the board 
   * fits the widget and stays centered, strictly keeping square cells.
   */
  void updateBoardLayout();

  /**
   * @brief Maps a grid position (row/col) to absolute pixel coordinates.
   * @param position Row and column of the cell in the maze matrix.
   * @return The exact pixel rectangle the cell occupies in the widget.
   */
  QRect cellRect(Position position) const;

  /**
   * @brief Draws the base layer of a single cell: floor/mud, special tiles, 
   * power-ups, portals, and boundary walls.
   * @param painter Painter already active on this widget.
   * @param position Row and column of the cell.
   */
  void drawCell(QPainter& painter, Position position);

  /**
   * @brief Draws the treasure icon dynamically scaled to its cell.
   * @param painter Painter already active on this widget.
   */
  void drawTreasure(QPainter& painter) const;

  /**
   * @brief Draws both players, marking the active one with an indicator ring.
   * @param painter Painter already active on this widget.
   * @note Players whose position is outside the maze bounds are skipped.
   */
  void drawPlayers(QPainter& painter) const;
 
  GameState state_;
  int cellSize_ = 0;
  QPoint boardOrigin_;
};
