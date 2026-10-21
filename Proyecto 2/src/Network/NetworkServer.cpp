// Copyright Alejandro, Ashley y Kevin
#include "NetworkServer.hpp"

#include <stdexcept>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>

#include "GameAction.hpp"
#include "GameEngine.hpp"
#include "GameState.hpp"
#include "NetworkProtocol.hpp"

NetworkServer::NetworkServer(GameEngine& engine, QObject* parent)
    : QObject(parent), engine_(engine), listener_(new QTcpServer(this)) {
  QObject::connect(listener_, &QTcpServer::newConnection, this, [this]() {
    acceptConnection();
  });
}

bool NetworkServer::start(uint16_t port) {
  return listener_->listen(QHostAddress::Any, port);
}

void NetworkServer::sendTo(int clientId, const std::string& message) {
  if (clientId < 0 || clientId >= 2 || !clients_[clientId]) return;

  std::string frame = frameNetworkMessage(message);
  clients_[clientId]->write(frame.data(), static_cast<qint64>(frame.size()));
}

void NetworkServer::acceptConnection() {
  while (listener_->hasPendingConnections()) {
    QTcpSocket* socket = listener_->nextPendingConnection();

    int id = clients_[0] == nullptr ? 0 : (clients_[1] == nullptr ? 1 : -1);

    if (id < 0 || started_) {
      std::string frame = frameNetworkMessage(serializeErrorMessage("Match is full"));

      socket->write(frame.data(), static_cast<qint64>(frame.size()));
      socket->flush();
      socket->disconnectFromHost();

      QObject::connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
      continue;
    }

    clients_[id] = socket;
    receiveBuffers_[id].clear();
    ready_[id] = false;

    QObject::connect(socket, &QTcpSocket::readyRead, this, [this, id]() {
      receiveFrom(id);
    });

    QObject::connect(socket, &QTcpSocket::disconnected, this, [this, id]() {
      disconnectClient(id);
    });
  }
}

void NetworkServer::receiveFrom(int clientId) {
  QTcpSocket* socket = clients_[clientId];
  if (!socket) return;

  receiveBuffers_[clientId] += socket->readAll().toStdString();

  try {
    for (const std::string& message : extractNetworkMessages(receiveBuffers_[clientId])) {
      NetworkMessageType type = parseNetworkMessageType(message);

      if (type == NetworkMessageType::Hello && !ready_[clientId] && !started_) {
        ready_[clientId] = true;
        sendTo(clientId, serializeStartMessage(clientId));

        if (ready_[0] && ready_[1]) {
          started_ = true;
          broadcastState();
        }
      } else if (type == NetworkMessageType::Action && started_ && ready_[clientId]) {
        handleAction(clientId, parseActionMessage(message));
      } else {
        sendTo(clientId, serializeErrorMessage("Unexpected message or match not started"));
      }
    }
  } catch (const std::exception& error) {
    sendTo(clientId, serializeErrorMessage(error.what()));
    socket->disconnectFromHost();
  }
}

void NetworkServer::handleAction(int clientId, const Action& action) {
  if (!started_ || engine_.state().gameOver || clientId != engine_.state().currentPlayer) {
    sendTo(clientId, serializeErrorMessage("Not your turn or match unavailable"));
    return;
  }

  if (!engine_.submitAction(action)) {
    sendTo(clientId, serializeErrorMessage("Invalid action"));
    return;
  }

  broadcastState();

  if (engine_.state().gameOver && engine_.state().winner >= 0) {
    for (int id = 0; id < 2; ++id) {
      sendTo(id, serializeGameOverMessage(engine_.state().winner));
    }
  }
}

void NetworkServer::broadcastState() {
  for (int id = 0; id < 2; ++id) {
    if (clients_[id] && ready_[id]) {
      sendTo(id, serializeGameView(createGameView(engine_.state(), id)));
    }
  }
}

void NetworkServer::disconnectClient(int clientId) {
  QTcpSocket* socket = clients_[clientId];

  clients_[clientId] = nullptr;
  ready_[clientId] = false;
  receiveBuffers_[clientId].clear();

  if (socket) socket->deleteLater();

  if (started_) {
    int other = 1 - clientId;

    sendTo(other, serializeErrorMessage("Opponent disconnected"));

    started_ = false;
    ready_[other] = false;

    if (clients_[other]) {
      clients_[other]->disconnectFromHost();
    }
  }
}
