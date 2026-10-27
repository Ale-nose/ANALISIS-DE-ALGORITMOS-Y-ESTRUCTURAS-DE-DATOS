// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QDialog>

class QLabel;

/**
 * @brief Dialog shown when a match ends. It announces the winner and offers
 * two choices: start a new game or quit.
 */
class GameOverDialog : public QDialog {
  Q_OBJECT

 public:
  /**
   * @brief Builds the dialog.
   * @param singlePlayer True if the match was against the AI.
   * @param parent Window this dialog belongs to.
   */
  explicit GameOverDialog(bool singlePlayer, QWidget* parent = nullptr);

  /**
   * @brief Writes the winner on the dialog and shows it, waiting until the
   * user chooses.
   * @param winnerIndex PLAYER_1 or PLAYER_2.
   */
  void showWinner(int winnerIndex);

  /**
   * @brief Tells which button the user pressed.
   * @return True for "New game", false for "Quit".
   */
  bool newGameRequested() const;

 protected:
  /**
   * @brief Ignores Esc, so the dialog cannot be closed by accident.
   */
  void reject() override;

 private slots:
  /**
   * @brief Remembers that the user wants another match and closes.
   */
  void chooseNewGame();

  /**
   * @brief Remembers that the user wants to quit and closes.
   */
  void chooseQuit();

 private:
  bool singlePlayer_;
  bool newGameRequested_ = false;
  QLabel* winnerLabel_;
};
