// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <QObject>
#include <QString>

#include "GameAction.hpp"
#include "GameState.hpp"
#include "GameView.hpp"

/**
 * @brief Online match as the interface sees it: it hides the server, the
 * client and the sockets. The window only calls these methods and listens to
 * the signals.
 */
class OnlineSession : public QObject {
  Q_OBJECT

 public:
  /**
   * @brief Creates an online session.
   * @param parent Optional QObject parent.
   */
  explicit OnlineSession(QObject* parent = nullptr);

  ~OnlineSession() override;

  /**
   * @brief Creates the match from the menu options, starts the server and
   * joins it as Player 1.
   * @param options Options chosen by the host.
   * @param port Port where the server listens.
   * @return False if the match could not be created or the port is busy.
   */
  bool host(const GameOptions& options, uint16_t port);

  /**
   * @brief Connects to a host.
   * @param address IP or name of the host.
   * @param port Port of the host.
   * @return False if the address or the port are not valid.
   */
  bool join(const std::string& address, uint16_t port);

  /**
   * @brief Sends an action; the server checks the turn.
   * @param action Move or JumpWall requested by the player.
   * @return True if it was sent.
   */
  bool sendAction(const Action& action);

  /**
   * @brief Closes the connection and the server, if any. It can be called
   * at any time, even when nothing is open.
   */
  void close();

  /**
   * @brief State of the match, to write the game log.
   * @return The authoritative state if this is the host, nullptr otherwise.
   */
  const GameState* authoritativeState() const;

 private:
  struct Private;
  std::unique_ptr<Private> d_;

 signals:
  /// The server assigned the player (PLAYER_1 or PLAYER_2) to this client.
  void playerAssigned(int playerId);
  /// A new view of the match arrived (after every accepted action).
  void viewReceived(const GameView& view);
  /// The match ended with this winner.
  void matchEnded(int winner);
  /// Connection error before the match starts (refused, match full...).
  void errorOccurred(const QString& error);
  /// Message during the match that does not stop it (e.g. "Not your turn").
  void notice(const QString& message);
  /// The connection closed (the other player left, the network failed...).
  void connectionClosed();
};
