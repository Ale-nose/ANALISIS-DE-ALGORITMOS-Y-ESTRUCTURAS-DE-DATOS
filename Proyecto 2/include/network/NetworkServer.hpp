// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <QObject>

class QTcpServer;
class QTcpSocket;
class GameEngine;
struct Action;

class NetworkServer : public QObject {
 private:
  GameEngine& engine_;
  QTcpServer* listener_ = nullptr;
  std::array<QTcpSocket*, 2> clients_{};
  std::array<std::string, 2> receiveBuffers_{};
  std::array<bool, 2> ready_{};
  bool started_ = false;

  void acceptConnection();
  void receiveFrom(int clientId);
  void disconnectClient(int clientId);
  void sendTo(int clientId, const std::string& message);
  void broadcastState();

 public:
  explicit NetworkServer(GameEngine& engine, QObject* parent = nullptr);

  bool start(uint16_t port);
  void handleAction(int clientId, const Action& action);
};
