// Copyright Alejandro, Ashley y Kevin
#include "StartWindow.hpp"

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <random>

#include "UiTexts.hpp"
#include "MenuTheme.hpp"
#include "UiControls.hpp"

namespace {

/**
 * @brief Builds a titled box that stacks the given widgets vertically.
 * @param title Text shown on the border of the box.
 * @param children Widgets to add, in order; the box becomes their parent.
 * @return A new QGroupBox (ownership goes to the parent layout).
 */
QGroupBox* makeGroup(const char* title, const QList<QWidget*>& children) {
  auto* box = new QGroupBox(title);
  auto* layout = new QVBoxLayout(box);
  for (QWidget* child : children) {
    layout->addWidget(child);
  }
  return box;
}

}  // namespace

StartWindow::StartWindow(QWidget* parent) : QWidget(parent) {
  setWindowTitle(TEXT_WINDOW_TITLE);
  setMinimumWidth(UI_WINDOW_MIN_WIDTH);
  setStyleSheet(buildStyleSheet());
  setAttribute(Qt::WA_StyledBackground, true);

  auto* title = new QLabel(TEXT_TITLE);
  title->setObjectName("titleLabel");
  title->setAlignment(Qt::AlignCenter);

  auto* subtitle = new QLabel(TEXT_SUBTITLE);
  subtitle->setObjectName("subtitleLabel");
  subtitle->setAlignment(Qt::AlignCenter);

  singlePlayerButton_ = new QRadioButton(TEXT_MODE_SINGLE);
  localTwoPlayersButton_ = new QRadioButton(TEXT_MODE_LOCAL);
  hostButton_ = new QRadioButton(TEXT_MODE_HOST);
  joinButton_ = new QRadioButton(TEXT_MODE_JOIN);
  singlePlayerButton_->setChecked(true);

  dfsButton_ = new QRadioButton(TEXT_GENERATOR_DFS);
  bfsButton_ = new QRadioButton(TEXT_GENERATOR_BFS);
  dfsButton_->setChecked(true);

  fogCheckBox_ = new QCheckBox(TEXT_FOG);

  addressEdit_ = new QLineEdit(ONLINE_DEFAULT_ADDRESS);
  addressEdit_->setPlaceholderText(TEXT_PLACEHOLDER_ADDRESS);
  portSpin_ = new QSpinBox;
  portSpin_->setRange(ONLINE_PORT_MIN, ONLINE_PORT_MAX);
  portSpin_->setValue(ONLINE_DEFAULT_PORT);
  portSpin_->setPrefix(TEXT_PORT_PREFIX);

  auto* startButton = new QPushButton(TEXT_START);
  startButton->setObjectName("startButton");
  startButton->setCursor(Qt::PointingHandCursor);
  connect(startButton, &QPushButton::clicked, this, &StartWindow::startGame);

  generatorGroup_ = makeGroup(TEXT_GROUP_GENERATOR, {dfsButton_, bfsButton_});

  // Radio buttons in different group boxes are exclusive independently,
  // because each QGroupBox is its own parent widget.
  auto* layout = new QVBoxLayout(this);
  layout->setContentsMargins(UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN
    , UI_LAYOUT_MARGIN, UI_LAYOUT_MARGIN);
  layout->setSpacing(UI_LAYOUT_SPACING);
  layout->addWidget(title);
  layout->addWidget(subtitle);
  layout->addWidget(makeGroup(TEXT_GROUP_MODE, {singlePlayerButton_
    , localTwoPlayersButton_, hostButton_, joinButton_}));
  layout->addWidget(makeGroup(TEXT_GROUP_CONNECTION, {addressEdit_
    , portSpin_}));
  layout->addWidget(generatorGroup_);
  layout->addWidget(fogCheckBox_);
  layout->addWidget(startButton);

  connect(hostButton_, &QRadioButton::toggled, this
    , &StartWindow::updateOnlineFields);
  connect(joinButton_, &QRadioButton::toggled, this
    , &StartWindow::updateOnlineFields);
  updateOnlineFields();
}

void StartWindow::updateOnlineFields() {
  const bool joining = joinButton_->isChecked();
  const bool online = joining || hostButton_->isChecked();

  addressEdit_->setEnabled(joining);
  portSpin_->setEnabled(online);
  generatorGroup_->setEnabled(!joining);
  fogCheckBox_->setEnabled(!joining);
}

GameOptions StartWindow::selectedOptions() const {
  GameOptions options;  // rows, cols and rates keep their config defaults
  if (hostButton_->isChecked()) {
    options.mode = GameMode::ONLINE;
  } else {
    options.mode = singlePlayerButton_->isChecked()
      ? GameMode::SINGLE_PLAYER : GameMode::LOCAL_TWO_PLAYERS;
  }
  options.generator = dfsButton_->isChecked() ? GenerationAlgorithm::DFS
    : GenerationAlgorithm::BFS;
  options.fogEnabled = fogCheckBox_->isChecked();
  options.seed = std::random_device{}();  // new maze on every match
  return options;
}

void StartWindow::startGame() {
  const quint16 port = static_cast<quint16>(portSpin_->value());

  if (hostButton_->isChecked()) {
    emit hostRequested(selectedOptions(), port);
  } else if (joinButton_->isChecked()) {
    emit joinRequested(addressEdit_->text().trimmed(), port);
  } else {
    emit gameStartRequested(selectedOptions());
  }
}
