// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstdint>
#include <string>
#include <QObject>

class QTcpSocket;
struct Action;

class NetworkClient : public QObject {
 private:
  QTcpSocket* socket_ = nullptr;
  std::string receiveBuffer_;
  std::string latestState_;
  std::string lastError_;
  std::string gameOverMessage_;
  int playerId_ = -1;

  void receiveMessages();

 public:
  explicit NetworkClient(QObject* parent = nullptr);

  bool connectTo(const std::string& host, uint16_t port);
  bool sendAction(const Action& action);
  inline void disconnectFromServer() {
    socket_->disconnectFromHost();
  }

  inline int playerId() const {
    return playerId_;
  }

  inline std::string latestState() const {
    return latestState_;
  }

  inline std::string lastError() const {
    return lastError_;
  }

  inline std::string gameOverMessage() const {
    return gameOverMessage_;
  }
};
