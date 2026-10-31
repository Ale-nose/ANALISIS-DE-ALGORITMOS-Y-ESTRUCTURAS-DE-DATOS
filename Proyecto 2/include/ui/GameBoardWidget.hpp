// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QPixmap>
#include <QPoint>
#include <QRect>
#include <QSize>
#include <QSvgRenderer>
#include <QWidget>
#include <vector>

#include "GameState.hpp"

class QPainter;
class QPaintEvent;
struct GameView;

/**
 * @brief Widget that draws the maze, the players, the treasure and every game
 * element (mud, portals, power-ups and Shield Tiles) using the images of the
 * assets folder.
 * @note It only reads the GameState it was given. Call setGameState every
 * time the state changes to trigger a repaint.
 */

// Extra: animated A* search visualizer
struct SearchOverlay {
  std::vector<Position> opened;
  std::vector<Position> closed;
  std::vector<Position> path;
};

class GameBoardWidget : public QWidget {
 public:
  /**
   * @brief Creates an empty board that expands to fill its container and
   * loads the images.
   * @param parent Optional parent widget.
   */
  explicit GameBoardWidget(QWidget* parent = nullptr);

  /**
   * @brief Stores a copy of the state to draw and repaints the board.
   * @param state Current state of the match.
   */
  void setGameState(const GameState& state);

  /**
   * @brief Sets the player whose point of view the board shows. That player
   * is always drawn, even under fog.
   * @param playerIndex PLAYER_1 or PLAYER_2.
   */
  void setViewer(int playerIndex);

  /**
   * @brief Sets the cells the viewer can see. It is only used when Fog of War
   * is enabled in the state options.
   * @param visible Positions of the visible cells.
   */
  void setVisibleCells(const std::vector<Position>& visible);

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
   * @return Cell side in pixels (0 if the maze is empty or the area is too small).
   */
  int calculateCellSize(QSize availableSize) const;

  /**
   * @brief Size of the maze in cells.
   * @return A QSize where width is the number of columns and height the rows.
   */
  QSize gridSize() const;

  // EXTRA: animated A* search visualizer
  void setSearchOverlay(const SearchOverlay& overlay);
  void clearSearchOverlay();

  // EXTRA: Network
  /**
   * @brief Draws the view an online server sent. Only the cells the server
   * allowed this player to see are known; the rest is hidden by the fog.
   * @param view Authorized view of the match.
   */
  void setGameView(const GameView& view);

  /**
   * @brief What the board is drawing now (used by the HUD).
   * @return The state built from the last state or view received.
   */
  const GameState& state() const {
    return state_;
  }

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
   * @brief Tells whether a cell must be drawn.
   * @param position Row and column of the cell.
   * @return True if the cell is visible to the viewer.
   */
  bool isCellVisible(Position position) const;

  /**
   * @brief Image of a power-up.
   * @param type Kind of power-up (not NONE).
   * @return The renderer that draws that power-up.
   */
  QSvgRenderer& powerUpRenderer(PowerUpType type);

  /**
   * @brief Image of a player.
   * @param player PLAYER_1 or PLAYER_2.
   * @param shielded True to get the image with the shield aura.
   * @return The renderer that draws that player.
   */
  QSvgRenderer& playerRenderer(int player, bool shielded);

  /**
   * @brief Draws the base layer of a single cell: floor/mud, special tiles, 
   * power-ups, portals, and boundary walls.
   * @param painter Painter already active on this widget.
   * @param position Row and column of the cell.
   */
  void drawCell(QPainter& painter, Position position);

  /**
   * @brief Draws the fog image over a hidden cell, hiding walls, treasure,
   * power-ups and portals.
   * @param painter Painter already active on this widget.
   * @param position Row and column of the hidden cell.
   */
  void drawFog(QPainter& painter, Position position);

  /**
   * @brief Draws the treasure image scaled to its cell, if it is visible.
   * @param painter Painter already active on this widget.
   */
  void drawTreasure(QPainter& painter);

  /**
   * @brief Draws the players, marking the active one with a ring. Under fog
   * the viewer is always drawn and the rival only if it is visible.
   * @param painter Painter already active on this widget.
   * @note Players whose position is outside the maze bounds are skipped.
   */
  void drawPlayers(QPainter& painter);

  // EXTRA: animated A* search visualizer
  void drawSearchOverlay(QPainter& painter) const;

  GameState state_;
  int viewer_ = PLAYER_1;
  std::vector<Position> visibleCells_;
  int cellSize_ = 0;
  QPoint boardOrigin_;

  // Images (loaded once in the constructor)
  QSvgRenderer player1Renderer_;
  QSvgRenderer player1ShieldRenderer_;
  QSvgRenderer player2Renderer_;
  QSvgRenderer player2ShieldRenderer_;
  QSvgRenderer treasureRenderer_;
  QSvgRenderer portalRenderer_;
  QSvgRenderer shieldTileRenderer_;
  QSvgRenderer jumpWallRenderer_;
  QSvgRenderer doublePlayRenderer_;
  QSvgRenderer controlEnemyRenderer_;
  QSvgRenderer fogRenderer_;
  QSvgRenderer mudRenderer_;
  // The fog is drawn once per cell size and reused for every hidden cell
  QPixmap fogPixmap_;

  // EXTRA: animated A* search visualizer
  SearchOverlay searchOverlay_;
};
