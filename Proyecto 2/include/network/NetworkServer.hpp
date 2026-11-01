// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <QObject>
#include <array>
#include <cstdint>
#include <string>

class QTcpServer;
class QTcpSocket;
class GameEngine;
struct Action;

/**
 * @brief Manages authoritative game hosting and TCP client connections.
 */
class NetworkServer : public QObject {
 private:
  GameEngine&
      engine_;  ///< Reference to the authoritative game engine instance.
  QTcpServer* listener_ = nullptr;  ///< TCP listening server socket.
  std::array<QTcpSocket*, 2>
      clients_{};  ///< Sockets for Player 1 and Player 2 clients.
  std::array<std::string, 2>
      receiveBuffers_{};         ///< Per-client incoming byte stream buffers.
  std::array<bool, 2> ready_{};  ///< Readiness flags for connected clients.
  bool started_ =
      false;  ///< Indicates if the multiplayer match is currently running.

  /**
   * @brief Accepts incoming client TCP connection requests.
   */
  void acceptConnection();

  /**
   * @brief Reads and parses available data from a specific client socket.
   * @param clientId Client index (0 or 1).
   */
  void receiveFrom(int clientId);

  /**
   * @brief Handles clean disconnection and cleanup for a client.
   * @param clientId Client index (0 or 1).
   */
  void disconnectClient(int clientId);

  /**
   * @brief Transmits a framed network message to a specific client.
   * @param clientId Client index (0 or 1).
   * @param message Message payload to send.
   */
  void sendTo(int clientId, const std::string& message);

  /**
   * @brief Broadcasts customized board states to each client according to Fog
   * of War visibility.
   */
  void broadcastState();

 public:
  /**
   * @brief Constructs a network server.
   * @param engine Authoritative game engine instance.
   * @param parent Optional parent QObject for memory management.
   */
  explicit NetworkServer(GameEngine& engine, QObject* parent = nullptr);

  /**
   * @brief Starts listening for incoming client connections on the designated
   * port.
   * @param port Port number to bind and listen on.
   * @return true if the server started listening successfully, false otherwise.
   */
  bool start(uint16_t port);

  /**
   * @brief Validates and applies an action received from a network client.
   * @param clientId Originating client index (0 or 1).
   * @param action Action submitted by the client.
   */
  void handleAction(int clientId, const Action& action);
};
