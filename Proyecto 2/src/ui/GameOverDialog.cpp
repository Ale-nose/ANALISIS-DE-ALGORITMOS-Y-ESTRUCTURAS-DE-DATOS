// Copyright Alejandro, Ashley y Kevin
#include "GameOverDialog.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
 
#include "GameConfig.hpp"
#include "UiTexts.hpp"
#include "MenuTheme.hpp"

GameOverDialog::GameOverDialog(bool singlePlayer, QWidget* parent)
  : QDialog(parent
  , Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint)
  , singlePlayer_(singlePlayer) {
  setWindowTitle(TEXT_WINDOW_TITLE);
  setModal(true);
  setStyleSheet(buildStyleSheet());
  setAttribute(Qt::WA_StyledBackground, true);
  setMinimumWidth(UI_WINDOW_MIN_WIDTH);

  auto* heading = new QLabel(TEXT_GAME_OVER_HEADING);
  heading->setObjectName("subtitleLabel");
  heading->setAlignment(Qt::AlignCenter);

  winnerLabel_ = new QLabel;
  winnerLabel_->setObjectName("titleLabel");
  winnerLabel_->setAlignment(Qt::AlignCenter);

  auto* newGameButton = new QPushButton(TEXT_GAME_OVER_NEW_GAME);
  newGameButton->setObjectName("startButton");
  newGameButton->setCursor(Qt::PointingHandCursor);
  newGameButton->setDefault(true);
  connect(newGameButton, &QPushButton::clicked, this
    , &GameOverDialog::chooseNewGame);
  
  auto* quitButton = new QPushButton(TEXT_GAME_OVER_QUIT);
  quitButton->setObjectName("startButton");
  quitButton->setCursor(Qt::PointingHandCursor);
  connect(quitButton, &QPushButton::clicked, this, &GameOverDialog::chooseQuit);

  auto* buttons = new QHBoxLayout;
  buttons->setSpacing(UI_LAYOUT_SPACING);
  buttons->addWidget(newGameButton);
  buttons->addWidget(quitButton);

  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN
    , UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN);
  layout->setSpacing(UI_LAYOUT_SPACING);
  layout->addWidget(heading);
  layout->addWidget(winnerLabel_);
  layout->addLayout(buttons);
}

void GameOverDialog::showWinner(int winnerIndex) {
  const char* name = TEXT_HUD_PLAYER_2;
  if (winnerIndex == PLAYER_1) {
    name = TEXT_HUD_PLAYER_1;
  } else if (singlePlayer_) {
    name = TEXT_HUD_AI;
  }
 
  winnerLabel_->setText(QString::fromLatin1(TEXT_GAME_OVER_WINNER)
    .arg(QString::fromLatin1(name)));
  exec();
}

bool GameOverDialog::newGameRequested() const {
  return newGameRequested_;
}

void GameOverDialog::reject() {
  // Intentionally empty: only the two buttons close this dialog.
}

void GameOverDialog::chooseNewGame() {
  newGameRequested_ = true;
  accept();
}

void GameOverDialog::chooseQuit() {
  newGameRequested_ = false;
  done(QDialog::Rejected);
}
