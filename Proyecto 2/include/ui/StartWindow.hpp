// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QString>
#include <QWidget>
#include <QtGlobal>

#include "GameState.hpp"

class QCheckBox;
class QGroupBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;

/**
 * @brief Start menu: lets the user pick game mode, maze generator and
 * Fog of War, or host/join an online match.
 * It never creates the match; it only reports the choice through the
 * gameStartRequested, hostRequested and joinRequested signals.
 */
class StartWindow : public QWidget {
  Q_OBJECT

 public:
  /**
   * @brief Builds the menu widgets and applies the application style.
   * @param parent Optional parent widget (the window owns its children).
   */
  explicit StartWindow(QWidget* parent = nullptr);

  /**
   * @brief Reads the options currently selected in the menu.
   * Rows, columns and spawn rates keep their GameConfig defaults. The seed is
   * random so every match generates a different maze.
   * @return The GameOptions that describe the selected match (mode ONLINE
   * when hosting).
   */
  GameOptions selectedOptions() const;

 signals:
  /**
   * @brief Emitted when the user presses START in a local match.
   * @param options The options selected in the menu.
   */
  void gameStartRequested(const GameOptions& options);

  // EXTRA: Network
  /**
   * @brief Emitted when the user presses START with "host" selected.
   * @param options Options of the match that will be hosted.
   * @param port Port where the server listens.
   */
  void hostRequested(const GameOptions& options, quint16 port);

  /**
   * @brief Emitted when the user presses START with "join" selected.
   * @param address Address of the host (IP or name).
   * @param port Port of the host.
   */
  void joinRequested(const QString& address, quint16 port);

 private slots:
  /**
   * @brief Emits the signal that matches the selected mode.
   */
  void startGame();

 private:
  /**
   * @brief Enables only the fields that the selected mode uses: the address
   * is for joining, the port for any online mode, and the generator and fog
   * are decided by the host, so joining disables them.
   */
  void updateOnlineFields();

  QRadioButton* singlePlayerButton_;
  QRadioButton* localTwoPlayersButton_;
  QRadioButton* hostButton_;
  QRadioButton* joinButton_;
  QRadioButton* dfsButton_;
  QRadioButton* bfsButton_;
  QCheckBox* fogCheckBox_;
  QGroupBox* generatorGroup_;
  QLineEdit* addressEdit_;
  QSpinBox* portSpin_;
};
