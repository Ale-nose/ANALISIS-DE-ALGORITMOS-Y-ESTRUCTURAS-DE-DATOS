// Copyright Alejandro, Ashley y Kevin
#include "GameBoardWidget.hpp"

#include <QColor>
#include <QDebug>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QString>
#include <algorithm>
#include <cstddef>
#include <utility>

#include "BoardTheme.hpp"
#include "GameView.hpp"
#include "UiAssets.hpp"
#include "UiTheme.hpp"

namespace {
/**
 * @brief Converts one of the "#RRGGBB" constants into a QColor.
 * @param hex Color text.
 * @return The matching QColor.
 */
QColor toColor(const char* hex) { return QColor(QLatin1String(hex)); }

/**
 * @brief Index of a direction inside Cell::walls.
 * @param direction Side of the cell.
 * @return Position of that side in the walls array.
 */
std::size_t wallSlot(Direction direction) {
  return static_cast<std::size_t>(direction);
}

/**
 * @brief Computes a smaller square centered inside a reference rectangle.
 * @param area Rectangle that provides the center coordinates.
 * @param ratio Fraction (0.0 to 1.0) of area's width used as the new side
 * length.
 * @return The perfectly centered square.
 */
QRectF centered(const QRectF& area, double ratio) {
  const double side = area.width() * ratio;

  // Offset the starting top-left corner by exactly half the side length
  // from the absolute center to maintain symmetry.
  return QRectF(area.center().x() - side / 2.0, area.center().y() - side / 2.0,
                side, side);
}

/**
 * @brief Draws bold text centered inside a bounding box.
 * @param painter Active painter.
 * @param area Rectangle where the text is horizontally and vertically centered.
 * @param text Text to draw.
 * @param color Text color.
 * @param pixelSize Font height in pixels, clamped to a minimum of 1.
 */
void drawLabel(QPainter& painter, const QRectF& area, const QString& text,
               const QColor& color, double pixelSize) {
  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(std::max(1, static_cast<int>(pixelSize)));
  painter.setFont(font);
  painter.setPen(color);
  painter.drawText(area, Qt::AlignCenter, text);
}

/**
 * @brief Loads an image from the embedded resources, warning if it fails.
 * @param renderer Renderer that will hold the image.
 * @param path Resource path (UiAssets.hpp).
 */
void loadSprite(QSvgRenderer& renderer, const char* path) {
  if (!renderer.load(QString::fromLatin1(path))) {
    qWarning() << "Could not load the image" << path;
  }
}

/**
 * @brief Draws an image centered in a cell.
 * @param painter Active painter.
 * @param renderer Image to draw.
 * @param area Cell rectangle.
 * @param ratio Side of the image as a fraction of the cell side.
 */
void drawSprite(QPainter& painter, QSvgRenderer& renderer, const QRectF& area,
                double ratio) {
  renderer.render(&painter, centered(area, ratio));
}

}  // namespace

GameBoardWidget::GameBoardWidget(QWidget* parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

  loadSprite(player1Renderer_, ASSET_PLAYER_1);
  loadSprite(player1ShieldRenderer_, ASSET_PLAYER_1_SHIELD);
  loadSprite(player2Renderer_, ASSET_PLAYER_2);
  loadSprite(player2ShieldRenderer_, ASSET_PLAYER_2_SHIELD);
  loadSprite(treasureRenderer_, ASSET_TREASURE);
  loadSprite(portalRenderer_, ASSET_PORTAL);
  loadSprite(shieldTileRenderer_, ASSET_SHIELD_TILE);
  loadSprite(jumpWallRenderer_, ASSET_JUMP_WALL);
  loadSprite(doublePlayRenderer_, ASSET_DOUBLE_PLAY);
  loadSprite(controlEnemyRenderer_, ASSET_CONTROL_ENEMY);
  loadSprite(fogRenderer_, ASSET_FOG);
  loadSprite(mudRenderer_, ASSET_MUD);
}

void GameBoardWidget::setGameState(const GameState& state) {
  state_ = state;
  updateGeometry();  // Informs Qt that size hints may have changed
  update();          // Triggers a full repaint using the new state
}

void GameBoardWidget::setViewer(int playerIndex) {
  viewer_ = playerIndex;
  update();
}

void GameBoardWidget::setVisibleCells(const std::vector<Position>& visible) {
  visibleCells_ = visible;
  update();
}

QSize GameBoardWidget::sizeHint() const {
  return QSize(
      state_.maze.cols() * BOARD_PREFERRED_CELL_SIZE + 2 * BOARD_MARGIN,
      state_.maze.rows() * BOARD_PREFERRED_CELL_SIZE + 2 * BOARD_MARGIN);
}

QSize GameBoardWidget::minimumSizeHint() const {
  return QSize(state_.maze.cols() * BOARD_MIN_CELL_SIZE + 2 * BOARD_MARGIN,
               state_.maze.rows() * BOARD_MIN_CELL_SIZE + 2 * BOARD_MARGIN);
}

int GameBoardWidget::calculateCellSize(QSize availableSize) const {
  const int cols = static_cast<int>(state_.maze.cols());
  const int rows = static_cast<int>(state_.maze.rows());
  if (cols == 0 || rows == 0) {
    return 0;
  }

  // Integer division gives the biggest whole cell for each direction
  const int widthPerCell = availableSize.width() / cols;
  const int heightPerCell = availableSize.height() / rows;
  // The smaller one keeps the cells square
  return std::max(0, std::min(widthPerCell, heightPerCell));
}

QSize GameBoardWidget::gridSize() const {
  return QSize(state_.maze.cols(), state_.maze.rows());
}

void GameBoardWidget::updateBoardLayout() {
  const Maze& maze = state_.maze;
  // Available area for the grid (subtracting margins from both sides)
  const QSize gridArea(width() - 2 * BOARD_MARGIN, height() - 2 * BOARD_MARGIN);

  // std::max to avoid a zero-size cell while the window is being resized
  cellSize_ = std::max(1, calculateCellSize(gridArea));

  // Leftover space divided by 2 centers the board in the window to leave equal
  // margins on all sides
  boardOrigin_ = QPoint((width() - cellSize_ * maze.cols()) / 2,
                        (height() - cellSize_ * maze.rows()) / 2);
}

QRect GameBoardWidget::cellRect(Position position) const {
  // Grid-to-pixel mapping: Origin + (Matrix Index * Cell Pixel Size)
  return QRect(boardOrigin_.x() + position.col * cellSize_,
               boardOrigin_.y() + position.row * cellSize_, cellSize_,
               cellSize_);
}

bool GameBoardWidget::isCellVisible(Position position) const {
  // Always true if Fog of War is off.
  if (!state_.options.fogEnabled) {
    return true;
  }

  return std::find(visibleCells_.begin(), visibleCells_.end(), position) !=
         visibleCells_.end();
}

QSvgRenderer& GameBoardWidget::powerUpRenderer(PowerUpType type) {
  switch (type) {
    case PowerUpType::DOUBLE_PLAY:
      return doublePlayRenderer_;
    case PowerUpType::CONTROL_ENEMY:
      return controlEnemyRenderer_;
    default:
      return jumpWallRenderer_;
  }
}

QSvgRenderer& GameBoardWidget::playerRenderer(int player, bool shielded) {
  if (player == PLAYER_1) {
    return shielded ? player1ShieldRenderer_ : player1Renderer_;
  }
  return shielded ? player2ShieldRenderer_ : player2Renderer_;
}

void GameBoardWidget::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
  painter.fillRect(rect(), toColor(COLOR_BOARD_BACKGROUND));

  updateBoardLayout();

  // Layer 1a: fog over every cell the viewer cannot see. It goes first so it
  // never covers the walls of a visible cell.
  for (int row = 0; row < state_.maze.rows(); ++row) {
    for (int col = 0; col < state_.maze.cols(); ++col) {
      const Position position{row, col};
      if (!isCellVisible(position)) {
        drawFog(painter, position);
      }
    }
  }

  // Layer 1b: base grid (floor, walls, static tiles) of the visible cells
  for (int row = 0; row < state_.maze.rows(); ++row) {
    for (int col = 0; col < state_.maze.cols(); ++col) {
      const Position position{row, col};
      if (isCellVisible(position)) {
        drawCell(painter, position);
      }
    }
  }

  // Layer 2: Dynamic entities (Treasure and Players go above every cell)
  drawTreasure(painter);
  drawSearchOverlay(painter);
  drawPlayers(painter);
}

void GameBoardWidget::drawCell(QPainter& painter, Position position) {
  const Cell& cell = state_.maze.cellAt(position);
  const QRectF area = cellRect(position);

  // Floor: flat color, mud is an image drawn on top of it
  painter.setPen(Qt::NoPen);
  painter.setBrush(toColor(COLOR_CELL_FLOOR));
  painter.drawRect(area);

  if (cell.terrain == TerrainType::MUD) {
    drawSprite(painter, mudRenderer_, area, BOARD_MUD_RATIO);
  }

  // Portal: image with the paired ID on a dark badge so the number is readable
  if (cell.portalID >= 0) {
    drawSprite(painter, portalRenderer_, area, BOARD_PORTAL_RATIO);

    painter.setPen(Qt::NoPen);
    painter.setBrush(toColor(COLOR_PORTAL_BADGE));
    painter.drawEllipse(centered(area, BOARD_PORTAL_BADGE_RATIO));

    drawLabel(painter, centered(area, BOARD_PORTAL_BADGE_RATIO),
              QString::number(cell.portalID), toColor(COLOR_PORTAL_LABEL),
              cellSize_ * BOARD_PORTAL_LABEL_RATIO);
  }

  // Shield Tile
  if (cell.special == SpecialCellType::SHIELD_TILE) {
    drawSprite(painter, shieldTileRenderer_, area, BOARD_SHIELD_RATIO);
  }

  // Power-up
  if (cell.powerUp != PowerUpType::NONE) {
    drawSprite(painter, powerUpRenderer(cell.powerUp), area,
               BOARD_POWER_UP_RATIO);
  }

  // Walls: line segments mapped to the 4 borders of the cell area
  // True indicates an intact wall; false is an open passage
  const double wallWidth =
      std::max(BOARD_MIN_WALL_WIDTH, cellSize_ * BOARD_WALL_RATIO);

  painter.setPen(QPen(toColor(COLOR_WALL), wallWidth, Qt::SolidLine,
                      Qt::SquareCap, Qt::MiterJoin));

  if (cell.walls[wallSlot(Direction::UP)]) {
    painter.drawLine(area.topLeft(), area.topRight());
  }
  if (cell.walls[wallSlot(Direction::DOWN)]) {
    painter.drawLine(area.bottomLeft(), area.bottomRight());
  }
  if (cell.walls[wallSlot(Direction::LEFT)]) {
    painter.drawLine(area.topLeft(), area.bottomLeft());
  }
  if (cell.walls[wallSlot(Direction::RIGHT)]) {
    painter.drawLine(area.topRight(), area.bottomRight());
  }
}

void GameBoardWidget::drawFog(QPainter& painter, Position position) {
  // The fog image is rendered once at the current cell size and reused
  if (fogPixmap_.width() != cellSize_) {
    fogPixmap_ = QPixmap(cellSize_, cellSize_);
    fogPixmap_.fill(Qt::transparent);

    QPainter fogPainter(&fogPixmap_);
    fogPainter.setRenderHint(QPainter::Antialiasing, true);
    fogRenderer_.render(&fogPainter, QRectF(0, 0, cellSize_, cellSize_));
  }
  painter.drawPixmap(cellRect(position).topLeft(), fogPixmap_);
}

void GameBoardWidget::drawTreasure(QPainter& painter) {
  if (!state_.maze.isInside(state_.treasure)) {
    return;
  }

  if (!isCellVisible(state_.treasure)) {
    return;  // hidden by the fog
  }

  drawSprite(painter, treasureRenderer_, cellRect(state_.treasure),
             BOARD_TREASURE_RATIO);
}

void GameBoardWidget::drawPlayers(QPainter& painter) {
  for (int player = 0; player < NUM_PLAYERS; ++player) {
    const Position position = state_.players[player].position;
    if (!state_.maze.isInside(position)) {
      continue;  // not placed yet (default position is -1, -1)
    }

    // Under fog the viewer is always seen, the rival only if close enough
    if (player != viewer_ && !isCellVisible(position)) {
      continue;
    }

    const QRectF area = cellRect(position);

    // Turn Indicator: ring behind the active player
    if (player == state_.currentPlayer && !state_.gameOver) {
      const double ringWidth = std::max(
          BOARD_MIN_OUTLINE_WIDTH, cellSize_ * BOARD_ACTIVE_RING_WIDTH_RATIO);

      painter.setBrush(Qt::NoBrush);
      painter.setPen(QPen(toColor(COLOR_ACTIVE_PLAYER_RING), ringWidth));
      painter.drawEllipse(centered(area, BOARD_ACTIVE_RING_RATIO));
    }

    // Player image, the one with the aura is used while a shield is held
    const bool shielded = state_.players[player].shieldCount > 0;
    drawSprite(painter, playerRenderer(player, shielded), area,
               BOARD_PLAYER_RATIO);
  }
}

// EXTRA: animated A* search visualizer
void GameBoardWidget::setSearchOverlay(const SearchOverlay& overlay) {
  searchOverlay_ = overlay;
  update();
}

void GameBoardWidget::clearSearchOverlay() {
  searchOverlay_ = SearchOverlay{};
  update();
}

// Extra: animated A* search visualizer
void GameBoardWidget::drawSearchOverlay(QPainter& painter) const {
  auto fillCells = [&](const std::vector<Position>& cells,
                       const QColor& color) {
    for (const Position& cell : cells) {
      QRectF rect(boardOrigin_.x() + cell.col * cellSize_,
                  boardOrigin_.y() + cell.row * cellSize_, cellSize_,
                  cellSize_);
      painter.fillRect(rect, color);
    }
  };
  fillCells(searchOverlay_.opened, COLOR_SEARCH_OPENED);
  fillCells(searchOverlay_.closed, COLOR_SEARCH_CLOSED);
  fillCells(searchOverlay_.path, COLOR_SEARCH_PATH);
}

// Extra: Network
void GameBoardWidget::setGameView(const GameView& view) {
  GameState state;  // only what the server let this player see
  state.maze = Maze(view.rows, view.cols);
  state.options.mode = GameMode::ONLINE;
  state.options.fogEnabled = true;  // hide every cell that was not received
  state.options.rows = view.rows;
  state.options.cols = view.cols;
  state.treasure = view.treasure;
  state.currentPlayer = view.currentPlayer;
  state.gameOver = view.gameOver;
  state.winner = view.winner;

  std::vector<Position> visible;
  visible.reserve(view.visibleCells.size());
  for (const VisibleCell& cell : view.visibleCells) {
    state.maze.cellAt(cell.position) = cell.contents;
    visible.push_back(cell.position);
  }
  for (int id = 0; id < NUM_PLAYERS; ++id) {
    // (-1, -1) if hidden
    state.players[id].position = view.players[id].position;
  }

  state_ = std::move(state);
  visibleCells_ = std::move(visible);
  updateGeometry();
  update();
}
