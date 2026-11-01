// Copyright Alejandro, Ashley y Kevin

#include "AStarVisualizer.hpp"

#include <cstddef>
#include <vector>

#include "GameBoardWidget.hpp"
#include "UiControls.hpp"

AStarVisualizer::AStarVisualizer(GameBoardWidget* board, QObject* parent)
    : QObject(parent), board_(board) {
  connect(&timer_, &QTimer::timeout, this, &AStarVisualizer::showNextStep);
}

void AStarVisualizer::playTrace(const SearchTrace& trace) {
  stopAnimation();
  trace_ = trace;
  shownClosed_ = 0;
  if (trace_.closed.empty()) {
    return;
  }
  timer_.start(SEARCH_STEP_INTERVAL_MS);
}

void AStarVisualizer::stopAnimation() {
  timer_.stop();
  board_->clearSearchOverlay();
}

bool AStarVisualizer::isRunning() const { return timer_.isActive(); }

void AStarVisualizer::showNextStep() {
  ++shownClosed_;
  const std::size_t openedCount = trace_.openedCountAtClose[shownClosed_ - 1];

  SearchOverlay overlay;
  overlay.closed.assign(
      trace_.closed.begin(),
      trace_.closed.begin() + static_cast<std::ptrdiff_t>(shownClosed_));
  overlay.opened.assign(
      trace_.opened.begin(),
      trace_.opened.begin() + static_cast<std::ptrdiff_t>(openedCount));

  const bool isLastStep = shownClosed_ >= trace_.closed.size();
  if (isLastStep) {
    overlay.path = trace_.path;
    timer_.stop();
  }
  board_->setSearchOverlay(overlay);
  if (isLastStep) {
    emit finished();
  }
}
