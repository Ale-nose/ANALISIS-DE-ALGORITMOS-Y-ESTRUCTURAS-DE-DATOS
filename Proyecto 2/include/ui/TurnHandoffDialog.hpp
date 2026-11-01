// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QDialog>
#include <QString>

/**
 * @brief Full-cover screen shown between turns in a local two-player match.
 * It shows only who plays next and a Ready button, so the previous player's
 * view of the maze is never visible to the next one.
 */
class TurnHandoffDialog : public QDialog {
  Q_OBJECT

 public:
  /**
   * @brief Builds the screen for the player who is about to play.
   * @param playerName Name of the next player, as shown in the HUD.
   * @param parent Window this dialog belongs to.
   */
  explicit TurnHandoffDialog(const QString& playerName,
                             QWidget* parent = nullptr);

 protected:
  /**
   * @brief The dialog can only be closed with the Ready button, so the board
   * is never revealed by accident.
   */
  void reject() override;

 private slots:
  /**
   * @brief Closes the dialog when the next player presses Ready.
   */
  void confirmReady();
};
