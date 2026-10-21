// Copyright Alejandro, Ashley y Kevin
#include "GameBoardWidget.hpp"

#include <QColor>
#include <QFont>
#include <QPaintEvent>
#include <QPainter>
#include <QPen>
#include <QPolygonF>
#include <QSizePolicy>
#include <QString>
#include <algorithm>
#include <cstddef>

#include "BoardTheme.hpp"
#include "UiTexts.hpp"

namespace {
/**
 * @brief Converts one of the "#RRGGBB" constants into a QColor.
 * @param hex Color text.
 * @return The matching QColor.
 */
QColor toColor(const char* hex) {
  return QColor(QLatin1String(hex));
}

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
 * @param ratio Fraction (0.0 to 1.0) of area's width used as the new side length.
 * @return The perfectly centered square.
 */
QRectF centered(const QRectF& area, double ratio) {
  const double side = area.width() * ratio;

  // Offset the starting top-left corner by exactly half the side length 
  // from the absolute center to maintain symmetry.
  return QRectF(area.center().x() - side / 2.0, area.center().y() - side 
    / 2.0, side, side);
}

/**
 * @brief Draws bold text perfectly centered inside a bounding box.
 * @param painter Active painter.
 * @param area Rectangle where the text is horizontally and vertically centered.
 * @param text Text to draw.
 * @param color Text color.
 * @param pixelSize Font height in pixels, clamped to a minimum of 1.
 */
void drawLabel(QPainter& painter, const QRectF& area, const QString& text
    , const QColor& color, double pixelSize) {
  QFont font = painter.font();
  font.setBold(true);
  font.setPixelSize(std::max(1, static_cast<int>(pixelSize)));
  painter.setFont(font);
  painter.setPen(color);
  painter.drawText(area, Qt::AlignCenter, text);
}

/**
 * @brief Generates a 4-point diamond polygon based on a bounding box.
 * @param box Square the diamond must fit strictly inside.
 * @return The diamond geometry, connecting the exact midpoints of the 4 edges.
 */
QPolygonF diamondShape(const QRectF& box) {
  const QPointF middle = box.center();
  QPolygonF diamond;

  // Map vertices starting top-middle, going clockwise: right, bottom, left.
  diamond << QPointF(middle.x(), box.top()) << QPointF(box.right(), middle.y())
    << QPointF(middle.x(), box.bottom()) << QPointF(box.left(), middle.y());

  return diamond;
}

/**
 * @brief Short letter shown on the badge of each power-up.
 * @param type Power-up kind.
 * @return The localized letter from UiTexts or an empty string for NONE.
 */
const char* powerUpLabel(PowerUpType type) {
  switch (type) {
    case PowerUpType::JUMP_WALL:
      return TEXT_BOARD_JUMP_WALL;
    case PowerUpType::DOUBLE_PLAY:
      return TEXT_BOARD_DOUBLE_PLAY;
    case PowerUpType::CONTROL_ENEMY:
      return TEXT_BOARD_CONTROL_ENEMY;
    case PowerUpType::NONE:
      break;
  }
  return "";
}

/**
 * @brief Color that identifies a player on the board.
 * @param player PLAYER_1 or PLAYER_2 index.
 * @return The corresponding hex color constant.
 */
const char* playerColor(int player) {
  return player == PLAYER_1 ? COLOR_PLAYER_1 : COLOR_PLAYER_2;
}

}  // namespace

GameBoardWidget::GameBoardWidget(QWidget* parent) : QWidget(parent) {
  setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

void GameBoardWidget::setGameState(const GameState& state) {
  state_ = state;
  updateGeometry();  // Informs Qt that size hints may have changed
  update();          // Triggers a full repaint using the new state
}

QSize GameBoardWidget::sizeHint() const {
  return QSize(state_.maze.cols() * BOARD_PREFERRED_CELL_SIZE + 2 * BOARD_MARGIN
    , state_.maze.rows() * BOARD_PREFERRED_CELL_SIZE + 2 * BOARD_MARGIN);
}

QSize GameBoardWidget::minimumSizeHint() const {
  return QSize(state_.maze.cols() * BOARD_MIN_CELL_SIZE + 2 * BOARD_MARGIN
    , state_.maze.rows() * BOARD_MIN_CELL_SIZE + 2 * BOARD_MARGIN);
}

int GameBoardWidget::calculateCellSize(QSize availableSize) const {
  if (state_.maze.cols() == 0 || state_.maze.rows() == 0) return 0;
  
  // Integer division rounds down, which gives the biggest whole size that fits
  const int widthPerCell = availableSize.width() / state_.maze.cols();
  const int heightPerCell = availableSize.height() / state_.maze.rows();
  // The smaller of the two limits keeps the cells square
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

  // Centers the grid by halving the leftover space: (Window Size - Grid Size) / 2
  // to leave equal margins on all sides
  boardOrigin_ = QPoint((width() - cellSize_ * maze.cols()) / 2, (height() 
    - cellSize_ * maze.rows()) / 2);
}

QRect GameBoardWidget::cellRect(Position position) const {
  // Absolute grid-to-pixel mapping: Origin + (Matrix Index * Cell Pixel Size)
  return QRect(boardOrigin_.x() + position.col * cellSize_, boardOrigin_.y() 
    + position.row * cellSize_, cellSize_, cellSize_);
}

void GameBoardWidget::paintEvent(QPaintEvent* event) {
  Q_UNUSED(event);

  QPainter painter(this);
  painter.setRenderHint(QPainter::Antialiasing, true);
  painter.fillRect(rect(), toColor(COLOR_BOARD_BACKGROUND));

  updateBoardLayout();
  
  // Layer 1: Base grid (Floor, walls, static tiles)
  for (int row = 0; row < state_.maze.rows(); ++row) {
    for (int col = 0; col < state_.maze.cols(); ++col) {
      drawCell(painter, Position{row, col});
    }
  }

  // Layer 2: Dynamic entities (Treasure and Players go above every cell)
  // TODO: (Fog of War issue) draw the fog layer after drawPlayers.
  drawTreasure(painter);
  drawPlayers(painter);
}

void GameBoardWidget::drawCell(QPainter& painter, Position position) {
  const Cell& cell = state_.maze.cellAt(position);
  const QRectF area = cellRect(position);

  // Dynamically scale outline width based on cell size to prevent thin lines on large screens
  const double outlineWidth = std::max(BOARD_MIN_OUTLINE_WIDTH, cellSize_ 
    * BOARD_OUTLINE_RATIO);

  // Floor: Flat coloring based on terrain type
  painter.setPen(Qt::NoPen);
  painter.setBrush(toColor(cell.terrain == TerrainType::MUD ? COLOR_MUD
    : COLOR_CELL_FLOOR));
  painter.drawRect(area);

  // Shield Tile: Centered square geometry with an 'S' label
  if (cell.special == SpecialCellType::SHIELD_TILE) {
    const QRectF square = centered(area, BOARD_SHIELD_RATIO);

    painter.setPen(QPen(toColor(COLOR_ICON_OUTLINE), outlineWidth));
    painter.setBrush(toColor(COLOR_SHIELD_TILE));
    painter.drawRect(square);

    drawLabel(painter, square, QString::fromLatin1(TEXT_BOARD_SHIELD_TILE)
      , toColor(COLOR_ICON_TEXT), cellSize_ * BOARD_LABEL_RATIO);
  }

  // Power-up Badge: Rounded rectangle to distinguish it from square shields
  if (cell.powerUp != PowerUpType::NONE) {
    const QRectF badge = centered(area, BOARD_POWER_UP_RATIO);
    const double corner = badge.width() * BOARD_BADGE_CORNER_RATIO;

    painter.setPen(QPen(toColor(COLOR_ICON_OUTLINE), outlineWidth));
    painter.setBrush(toColor(COLOR_POWER_UP));
    painter.drawRoundedRect(badge, corner, corner);

    drawLabel(painter, badge, QString::fromLatin1(powerUpLabel(cell.powerUp))
      , toColor(COLOR_ICON_TEXT), cellSize_ * BOARD_LABEL_RATIO);
  }

  // Portals: Two concentric rings enclosing the paired ID
  if (cell.portalID >= 0) {
    const QRectF outer = centered(area, BOARD_PORTAL_RATIO);
    const QRectF inner = centered(outer, BOARD_PORTAL_INNER_RATIO);

    const double ringWidth = std::max(BOARD_MIN_OUTLINE_WIDTH, cellSize_ 
      * BOARD_PORTAL_RING_RATIO);
      
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(toColor(COLOR_PORTAL), ringWidth));
    painter.drawEllipse(outer);
    painter.drawEllipse(inner);

    drawLabel(painter, inner, QString::number(cell.portalID)
      , toColor(COLOR_PORTAL), cellSize_ * BOARD_PORTAL_LABEL_RATIO);
  }

  // Walls: Line segments mapped to the 4 borders of the cell area.
  // True indicates an intact wall; false is an open passage.
  const double wallWidth = std::max(BOARD_MIN_WALL_WIDTH, cellSize_ 
    * BOARD_WALL_RATIO);

  painter.setPen(QPen(toColor(COLOR_WALL), wallWidth, Qt::SolidLine
    , Qt::SquareCap, Qt::MiterJoin));

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

void GameBoardWidget::drawTreasure(QPainter& painter) const {
  if (!state_.maze.isInside(state_.treasure)) {
    return;
  }

  // Bind the diamond strictly to the treasure's cell dimensions
  const QRectF box = centered(cellRect(state_.treasure), BOARD_TREASURE_RATIO);
  const double outlineWidth = std::max(BOARD_MIN_OUTLINE_WIDTH, cellSize_ 
    * BOARD_OUTLINE_RATIO);

  painter.setPen(QPen(toColor(COLOR_ICON_OUTLINE), outlineWidth));
  painter.setBrush(toColor(COLOR_TREASURE));
  painter.drawPolygon(diamondShape(box));
}

void GameBoardWidget::drawPlayers(QPainter& painter) const {
  const double outlineWidth = std::max(BOARD_MIN_OUTLINE_WIDTH, cellSize_ 
    * BOARD_OUTLINE_RATIO);

  for (int player = 0; player < NUM_PLAYERS; ++player) {
    const Position position = state_.players[player].position;
    if (!state_.maze.isInside(position)) {
      continue;  // not placed yet (default position is -1, -1)
    }

    const QRectF area = cellRect(position);

    // Turn Indicator: Outer ring wrapping the active player
    if (player == state_.currentPlayer && !state_.gameOver) {
      const double ringWidth = std::max(BOARD_MIN_OUTLINE_WIDTH, cellSize_
        * BOARD_ACTIVE_RING_WIDTH_RATIO);

      painter.setBrush(Qt::NoBrush);
      painter.setPen(QPen(toColor(COLOR_ACTIVE_PLAYER_RING), ringWidth));
      painter.drawEllipse(centered(area, BOARD_ACTIVE_RING_RATIO));
    }

    // Player Avatar: Central colored disc
    const QRectF disc = centered(area, BOARD_PLAYER_RATIO);

    painter.setPen(QPen(toColor(COLOR_ICON_OUTLINE), outlineWidth));
    painter.setBrush(toColor(playerColor(player)));
    painter.drawEllipse(disc);

    drawLabel(painter, disc, QString::number(player + 1)
      , toColor(COLOR_ICON_TEXT), cellSize_ * BOARD_LABEL_RATIO);
  }
}
