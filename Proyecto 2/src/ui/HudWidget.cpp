// Copyright Alejandro, Ashley y Kevin
#include "HudWidget.hpp"
 
#include <QLabel>
#include <QString>
#include <QStringList>
#include <QVBoxLayout>
 
#include "HudTheme.hpp"
#include "UiTexts.hpp"
#include "UiTheme.hpp"

namespace {

/**
 * @brief Name shown for a player. In single player mode the second player is
 * the AI.
 * @param state Current match state.
 * @param index PLAYER_1 or PLAYER_2.
 * @return The name to show in the HUD.
 */
QString playerName(const GameState& state, int index) {
  if (index == PLAYER_1) {
    return QString::fromLatin1(TEXT_HUD_PLAYER_1);
  }
  if (state.options.mode == GameMode::SINGLE_PLAYER) {
    return QString::fromLatin1(TEXT_HUD_AI);
  }

  return QString::fromLatin1(TEXT_HUD_PLAYER_2);
}

/**
 * @brief Style of the title label: big, bold and in the player's color.
 * @param color Hex color of the player.
 * @return A QSS snippet for the title label.
 */
QString titleStyle(const char* color) {
  return QString("font-size: %1px; font-weight: bold; color: %2;")
    .arg(HUD_TITLE_FONT_SIZE).arg(QString::fromLatin1(color));
}

QString countText(const char* format, int value, bool hidden) {
  const QString shown = hidden ? QString::fromLatin1(TEXT_HUD_HIDDEN)
    : QString::number(value);
  return QString::fromLatin1(format).arg(shown);
}

}  // namespace

HudWidget::HudWidget(QWidget* parent) : QWidget(parent) {
  setObjectName("hudPanel");
  setAttribute(Qt::WA_StyledBackground, true);
  setFixedWidth(HUD_WIDTH);

  const QString panelStyle = QString("QWidget#hudPanel { background-color: %1; " 
    "border-left: %2px solid %3; } QLabel { background-color: transparent; }")
    .arg(QString::fromLatin1(COLOR_HUD_PANEL))
    .arg(HUD_BORDER_WIDTH)
    .arg(QString::fromLatin1(COLOR_HUD_BORDER));
  
  setStyleSheet(buildStyleSheet() + panelStyle);

  playerLabel_ = new QLabel;
  actionsLabel_ = new QLabel;
  jumpWallsLabel_ = new QLabel;
  shieldsLabel_ = new QLabel;

  noticeLabel_ = new QLabel;
  noticeLabel_->setWordWrap(true);
  noticeLabel_->setStyleSheet(QString("color: %1;")
    .arg(QString::fromLatin1(COLOR_HUD_NOTICE)));
  
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(HUD_MARGIN, HUD_MARGIN, HUD_MARGIN, HUD_MARGIN);
  layout->setSpacing(HUD_SPACING);
  layout->addWidget(playerLabel_);
  layout->addWidget(actionsLabel_);
  layout->addWidget(jumpWallsLabel_);
  layout->addWidget(shieldsLabel_);
  layout->addWidget(noticeLabel_);
  layout->addStretch();
}

void HudWidget::updateFromState(const GameState& state) {
  const int current = state.currentPlayer;
  const int opponent = (current + 1) % NUM_PLAYERS;
  const PlayerState& player = state.players[current];

  playerLabel_->setText(QString::fromLatin1(TEXT_HUD_TURN)
    .arg(playerName(state, current)));
  playerLabel_->setStyleSheet(titleStyle(current == PLAYER_1 ? COLOR_PLAYER_1 
    : COLOR_PLAYER_2));
  
  actionsLabel_->setText(countText(TEXT_HUD_ACTIONS, state.actionsLeft, false));
  jumpWallsLabel_->setText(countText(TEXT_HUD_JUMP_WALLS, player.jumpWalls
    , privateInfoHidden_));
  shieldsLabel_->setText(countText(TEXT_HUD_SHIELDS, player.shieldCount
    , privateInfoHidden_));
  
  // Warnings: pending mud actions of the current player, and an opponent
  // that will lose the next turn.
  QStringList notices;
  if (player.pendingMudActions > 0) {
    notices << QString::fromLatin1(TEXT_HUD_MUD).arg(player.pendingMudActions);
  }
  if (state.players[opponent].skipNextTurn) {
    notices << QString::fromLatin1(TEXT_HUD_OPPONENT_SKIPS);
  }

  noticeLabel_->setText(notices.join("\n"));
}

void HudWidget::setPrivateInfoHidden(bool hidden) {
  privateInfoHidden_ = hidden;
}
