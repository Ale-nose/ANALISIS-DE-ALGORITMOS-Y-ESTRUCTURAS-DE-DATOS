// Copyright Alejandro, Ashley y Kevin
#include "NetworkClient.hpp"

#include <QAbstractSocket>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QtGlobal>
#include <stdexcept>

#include "GameAction.hpp"
#include "NetworkProtocol.hpp"

NetworkClient::NetworkClient(QObject* parent)
    : QObject(parent), socket_(new QTcpSocket(this)) {
  QObject::connect(socket_, &QTcpSocket::connected, this, [this]() {
    const std::string frame = frameNetworkMessage(serializeHelloMessage());

    const qint64 written =
        socket_->write(frame.data(), static_cast<qint64>(frame.size()));

    if (written != static_cast<qint64>(frame.size())) {
      lastError_ = "Could not send HELLO";
      emit errorReceived(QString::fromStdString(lastError_));
      socket_->disconnectFromHost();
    }
  });

  QObject::connect(socket_, &QTcpSocket::readyRead, this,
                   [this]() { receiveMessages(); });

  QObject::connect(socket_, &QTcpSocket::disconnected, this, [this]() {
    playerId_ = -1;
    emit connectionClosed();
  });

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
  QObject::connect(socket_, &QTcpSocket::errorOccurred, this,
                   [this](QAbstractSocket::SocketError) {
                     lastError_ = socket_->errorString().toStdString();
                     emit errorReceived(QString::fromStdString(lastError_));
                   });
#else
  QObject::connect(
      socket_, QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error),
      this, [this](QAbstractSocket::SocketError) {
        lastError_ = socket_->errorString().toStdString();
        emit errorReceived(QString::fromStdString(lastError_));
      });
#endif
}

bool NetworkClient::connectTo(const std::string& host, uint16_t port) {
  if (host.empty() || port == 0 ||
      socket_->state() != QAbstractSocket::UnconnectedState) {
    return false;
  }

  playerId_ = -1;
  gameOver_ = false;

  receiveBuffer_.clear();
  latestState_.clear();
  lastError_.clear();
  gameOverMessage_.clear();

  socket_->connectToHost(QString::fromStdString(host), port);

  return true;
}

bool NetworkClient::sendAction(const Action& action) {
  if (socket_->state() != QAbstractSocket::ConnectedState || playerId_ < 0 ||
      gameOver_) {
    return false;
  }

  const std::string frame = frameNetworkMessage(serializeActionMessage(action));

  const qint64 written =
      socket_->write(frame.data(), static_cast<qint64>(frame.size()));

  return written == static_cast<qint64>(frame.size());
}

void NetworkClient::receiveMessages() {
  receiveBuffer_ += socket_->readAll().toStdString();

  try {
    const std::vector<std::string> messages =
        extractNetworkMessages(receiveBuffer_);

    for (const std::string& message : messages) {
      const NetworkMessageType type = parseNetworkMessageType(message);

      if (type == NetworkMessageType::Start) {
        const QJsonObject object =
            QJsonDocument::fromJson(QByteArray::fromStdString(message))
                .object();

        if (!object.value("playerId").isDouble()) {
          throw std::runtime_error("Missing player ID");
        }

        const int id = object.value("playerId").toInt(-1);

        if ((id != 0 && id != 1) || playerId_ != -1) {
          throw std::runtime_error("Invalid START message");
        }

        playerId_ = id;
        emit startReceived(playerId_);

      } else if (type == NetworkMessageType::State) {
        if (playerId_ < 0 || gameOver_) {
          throw std::runtime_error("Unexpected STATE message");
        }

        const GameView view = deserializeGameView(message);

        latestState_ = message;
        emit gameViewReceived(view);

      } else if (type == NetworkMessageType::Error) {
        const QJsonObject object =
            QJsonDocument::fromJson(QByteArray::fromStdString(message))
                .object();

        if (!object.value("reason").isString()) {
          throw std::runtime_error("Invalid ERROR message");
        }

        const QString reason = object.value("reason").toString();

        lastError_ = reason.toStdString();
        emit errorReceived(reason);

      } else if (type == NetworkMessageType::GameOver) {
        const QJsonObject object =
            QJsonDocument::fromJson(QByteArray::fromStdString(message))
                .object();

        if (!object.value("winner").isDouble()) {
          throw std::runtime_error("Missing winner");
        }

        const int winner = object.value("winner").toInt(-1);

        if (winner != 0 && winner != 1) {
          throw std::runtime_error("Invalid winner");
        }

        gameOver_ = true;
        gameOverMessage_ = message;

        emit gameOverReceived(winner);

      } else {
        throw std::runtime_error("Unexpected server message");
      }
    }
  } catch (const std::exception& error) {
    lastError_ = error.what();

    emit errorReceived(QString::fromStdString(lastError_));
    socket_->disconnectFromHost();
  }
}
