// Copyright Alejandro, Ashley y Kevin
#include "NetworkClient.hpp"

#include <QAbstractSocket>
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <stdexcept>

#include "GameAction.hpp"
#include "NetworkProtocol.hpp"

NetworkClient::NetworkClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this)) {
  QObject::connect(socket_, &QTcpSocket::readyRead, this, [this]() {
    receiveMessages();
  });

  QObject::connect(socket_, &QTcpSocket::disconnected, this, [this]() {
    playerId_ = -1;
  });
}

bool NetworkClient::connectTo(const std::string& host, uint16_t port) {
  if (socket_->state() != QAbstractSocket::UnconnectedState) return false;

  playerId_ = -1;
  receiveBuffer_.clear();
  latestState_.clear();
  lastError_.clear();
  gameOverMessage_.clear();

  socket_->connectToHost(QString::fromStdString(host), port);

  if (!socket_->waitForConnected(3000)) {
    lastError_ = socket_->errorString().toStdString();
    return false;
  }

  std::string hello = frameNetworkMessage(serializeHelloMessage());

  return socket_->write(hello.data(), static_cast<qint64>(hello.size()))
      == static_cast<qint64>(hello.size());
}

bool NetworkClient::sendAction(const Action& action) {
  if (socket_->state() != QAbstractSocket::ConnectedState || playerId_ < 0) {
    return false;
  }

  std::string frame = frameNetworkMessage(serializeActionMessage(action));

  return socket_->write(frame.data(), static_cast<qint64>(frame.size()))
      == static_cast<qint64>(frame.size());
}

void NetworkClient::receiveMessages() {
  receiveBuffer_ += socket_->readAll().toStdString();

  try {
    for (const std::string& message : extractNetworkMessages(receiveBuffer_)) {
      NetworkMessageType type = parseNetworkMessageType(message);

      if (type == NetworkMessageType::Start) {
        QJsonObject object = QJsonDocument::fromJson(QByteArray::fromStdString(message)).object();
        int id = object.value("playerId").toInt(-1);

        if (id != 0 && id != 1) {
          throw std::runtime_error("Invalid player ID");
        }

        playerId_ = id;
      } else if (type == NetworkMessageType::State) {
        latestState_ = message;
      } else if (type == NetworkMessageType::Error) {
        QJsonObject object = QJsonDocument::fromJson(QByteArray::fromStdString(message)).object();
        lastError_ = object.value("reason").toString().toStdString();
      } else if (type == NetworkMessageType::GameOver) {
        gameOverMessage_ = message;
      } else {
        throw std::runtime_error("Unexpected server message");
      }
    }
  } catch (const std::exception& error) {
    lastError_ = error.what();
    socket_->disconnectFromHost();
  }
}
