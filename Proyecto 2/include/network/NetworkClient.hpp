// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstdint>
#include <string>
#include <QObject>
#include <QTcpSocket>
#include <QString>

#include "GameView.hpp"

struct Action;

/**
 * @brief Manages the client-side TCP connection and communication with the game server.
 */
class NetworkClient : public QObject {
  Q_OBJECT

 private:
  QTcpSocket* socket_ = nullptr;        ///< TCP socket handling network I/O.
  std::string receiveBuffer_;           ///< Stream buffer for incoming framed network data.
  std::string latestState_;             ///< Most recent serialized GameView received.
  std::string lastError_;               ///< Last error message reported by the server or socket.
  std::string gameOverMessage_;         ///< Outcome message received upon match conclusion.
  int playerId_ = -1;                   ///< Assigned player index for this client (0 or 1).
  bool gameOver_ = false;

  /**
   * @brief Parses and processes complete delimited messages from the receive buffer.
   */
  void receiveMessages();

 public:
  /**
   * @brief Constructs a network client.
   * @param parent Optional parent QObject for memory management.
   */
  explicit NetworkClient(QObject* parent = nullptr);

  /**
   * @brief Establishes a TCP connection to the specified host and port.
   * @param host Server hostname or IP address.
   * @param port Server listening port.
   * @return true if the connection was initiated successfully, false otherwise.
   */
  bool connectTo(const std::string& host, uint16_t port);

  /**
   * @brief Serializes and transmits a player action to the server.
   * @param action The move or wall jump action to submit.
   * @return true if transmitted successfully, false otherwise.
   */
  bool sendAction(const Action& action);

  /**
   * @brief Disconnects the socket from the host server.
   */
  inline void disconnectFromServer() {
    socket_->disconnectFromHost();
  }

  /**
   * @brief Gets the assigned player index for this client session.
   * @return Player index (0 or 1), or -1 if not assigned.
   */
  inline int playerId() const {
    return playerId_;
  }

  /**
   * @brief Gets the latest serialized state payload received from the server.
   * @return Raw state string.
   */
  inline std::string latestState() const {
    return latestState_;
  }

  /**
   * @brief Gets the most recent error message recorded.
   * @return Error description string.
   */
  inline std::string lastError() const {
    return lastError_;
  }

  /**
   * @brief Gets the game over message received from the server.
   * @return Match conclusion details string.
   */
  inline std::string gameOverMessage() const {
    return gameOverMessage_;
  }

 signals:
  void connectionClosed();
  void errorReceived(const QString& error);
  void startReceived(int playerId);
  void gameViewReceived(const GameView& view);
  void gameOverReceived(int winner);
};
