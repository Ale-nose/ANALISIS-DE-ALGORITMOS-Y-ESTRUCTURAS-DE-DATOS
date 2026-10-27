// Copyright Alejandro, Ashley y Kevin
#include "TurnHandoffDialog.hpp"

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
 
#include "UiTexts.hpp"
#include "MenuTheme.hpp"

TurnHandoffDialog::TurnHandoffDialog(const QString& playerName, QWidget* parent)
  : QDialog(parent, Qt::Dialog | Qt::FramelessWindowHint) {
  setModal(true);
  setStyleSheet(buildStyleSheet());
  setAttribute(Qt::WA_StyledBackground, true);

  auto* message = new QLabel(TEXT_HANDOFF_MESSAGE);
  message->setObjectName("subtitleLabel");
  message->setAlignment(Qt::AlignCenter);
 
  auto* name = new QLabel(playerName);
  name->setObjectName("titleLabel");
  name->setAlignment(Qt::AlignCenter);
 
  auto* readyButton = new QPushButton(TEXT_HANDOFF_READY);
  readyButton->setObjectName("startButton");
  readyButton->setCursor(Qt::PointingHandCursor);
  readyButton->setDefault(true);
  connect(readyButton, &QPushButton::clicked, this
    , &TurnHandoffDialog::confirmReady);
 
  // Stretches above and below keep everything centered on the screen.
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN
    , UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN);
  layout->setSpacing(UI_LAYOUT_SPACING);
  layout->addStretch(1);
  layout->addWidget(message);
  layout->addWidget(name);
  layout->addWidget(readyButton, 0, Qt::AlignHCenter);
  layout->addStretch(1);
}

void TurnHandoffDialog::reject() {
  // Intentionally empty: only confirmReady() closes this dialog.
}
 
void TurnHandoffDialog::confirmReady() {
  accept();
}
