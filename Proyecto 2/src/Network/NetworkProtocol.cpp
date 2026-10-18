// Copyright Alejandro, Ashley and Kevin
#include "NetworkProtocol.hpp"

#include <cstdint>
#include <stdexcept>
#include <utility>

#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include "GameAction.hpp"

QJsonObject readJsonObject(const std::string& message) {
  if (message.empty() || message.size() > MAX_NETWORK_MESSAGE_SIZE) {
    throw std::invalid_argument("Invalid network message size");
  }

  QJsonParseError error;
  QByteArray bytes(message.data(), static_cast<int>(message.size()));
  QJsonDocument document = QJsonDocument::fromJson(bytes, &error);
  if (error.error != QJsonParseError::NoError || !document.isObject()) {
    throw std::invalid_argument("Network message must be a valid JSON object");
  }
  return document.object();
}

std::string writeJsonObject(const QJsonObject& object) {
  return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}

QString directionToString(Direction direction) {
  switch (direction) {
    case Direction::UP:
      return QStringLiteral("UP");
    case Direction::DOWN:
      return QStringLiteral("DOWN");
    case Direction::LEFT:
      return QStringLiteral("LEFT");
    case Direction::RIGHT:
      return QStringLiteral("RIGHT");
  }
  throw std::invalid_argument("Unknown action direction");
}

Direction stringToDirection(const QString& direction) {
  if (direction == QStringLiteral("UP")) {
    return Direction::UP;
  }
  if (direction == QStringLiteral("DOWN")) {
    return Direction::DOWN;
  }
  if (direction == QStringLiteral("LEFT")) {
    return Direction::LEFT;
  }
  if (direction == QStringLiteral("RIGHT")) {
    return Direction::RIGHT;
  }
  throw std::invalid_argument("Unknown action direction");
}

std::string serializeHelloMessage() {
  QJsonObject message;
  message["type"] = QStringLiteral("HELLO");
  message["version"] = NETWORK_PROTOCOL_VERSION;
  return writeJsonObject(message);
}

std::string serializeStartMessage(int playerId) {
  if (playerId != 0 && playerId != 1) {
    throw std::invalid_argument("Invalid player ID");
  }
  QJsonObject message;
  message["type"] = QStringLiteral("START");
  message["playerId"] = playerId;
  return writeJsonObject(message);
}

std::string serializeActionMessage(const Action& action) {
  QString kind;
  switch (action.type) {
    case ActionType::Move:
      kind = QStringLiteral("MOVE");
      break;
    case ActionType::JumpWall:
      kind = QStringLiteral("JUMP_WALL");
      break;
    default: throw std::invalid_argument("Unknown action type");
  }

  QJsonObject actionObject;
  actionObject["kind"] = kind;
  actionObject["direction"] = directionToString(action.direction);

  QJsonObject message;
  message["type"] = QStringLiteral("ACTION");
  message["action"] = actionObject;
  return writeJsonObject(message);
}

std::string serializeErrorMessage(const std::string& reason) {
  if (reason.empty()) {
    throw std::invalid_argument("Error reason cannot be empty");
  }
  QJsonObject message;
  message["type"] = QStringLiteral("ERROR");
  message["reason"] = QString::fromStdString(reason);
  return writeJsonObject(message);
}

std::string serializeGameOverMessage(int winner) {
  if (winner != 0 && winner != 1) {
    throw std::invalid_argument("Invalid winner ID");
  }
  QJsonObject message;
  message["type"] = QStringLiteral("GAME_OVER");
  message["winner"] = winner;
  return writeJsonObject(message);
}

NetworkMessageType parseNetworkMessageType(const std::string& message) {
  QJsonValue type = readJsonObject(message).value("type");
  if (!type.isString()) {
    throw std::invalid_argument("Network message type is missing");
  }

  QString name = type.toString();
  if (name == QStringLiteral("HELLO")) {
    return NetworkMessageType::Hello;
  }
  if (name == QStringLiteral("START")) {
    return NetworkMessageType::Start;
  }
  if (name == QStringLiteral("ACTION")) {
    return NetworkMessageType::Action;
  }
  if (name == QStringLiteral("STATE")) {
    return NetworkMessageType::State;
  }
  if (name == QStringLiteral("ERROR")) {
    return NetworkMessageType::Error;
  }
  if (name == QStringLiteral("GAME_OVER")) {
    return NetworkMessageType::GameOver;
  }
  throw std::invalid_argument("Unknown network message type");
}

Action parseActionMessage(const std::string& message) {
  QJsonObject root = readJsonObject(message);
  if (!root.value("type").isString() || root.value("type").toString() != QStringLiteral("ACTION")) {
    throw std::invalid_argument("Expected an ACTION message");
  }
  if (!root.value("action").isObject()) {
    throw std::invalid_argument("Missing action object");
  }

  QJsonObject data = root.value("action").toObject();
  if (!data.value("kind").isString() || !data.value("direction").isString()) {
    throw std::invalid_argument("Invalid action fields");
  }

  Action action{};
  QString kind = data.value("kind").toString();
  if (kind == QStringLiteral("MOVE")) {
    action.type = ActionType::Move;
  } else if (kind == QStringLiteral("JUMP_WALL")) {
    action.type = ActionType::JumpWall;
  } else {
    throw std::invalid_argument("Unknown action type");
  }

  action.direction = stringToDirection(data.value("direction").toString());
  return action;
}

std::string frameNetworkMessage(const std::string& message) {
  parseNetworkMessageType(message);
  std::uint32_t size = static_cast<std::uint32_t>(message.size());
  std::string frame(4, '\0');
  frame[0] = static_cast<char>((size >> 24) & 0xFF);
  frame[1] = static_cast<char>((size >> 16) & 0xFF);
  frame[2] = static_cast<char>((size >> 8) & 0xFF);
  frame[3] = static_cast<char>(size & 0xFF);
  frame += message;
  return frame;
}

std::vector<std::string> extractNetworkMessages(std::string& receiveBuffer) {
  std::vector<std::string> messages;
  std::size_t offset = 0;

  while (receiveBuffer.size() - offset >= 4) {
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(receiveBuffer.data() + offset);
    std::uint32_t length = (static_cast<std::uint32_t>(bytes[0]) << 24) | (static_cast<std::uint32_t>(bytes[1]) << 16)
      | (static_cast<std::uint32_t>(bytes[2]) << 8) | static_cast<std::uint32_t>(bytes[3]);

    if (length == 0 || length > MAX_NETWORK_MESSAGE_SIZE) {
      throw std::invalid_argument("Invalid TCP frame length");
    }
    if (receiveBuffer.size() - offset - 4 < length) {
      break;
    }

    std::string message = receiveBuffer.substr(offset + 4, length);
    parseNetworkMessageType(message);
    messages.push_back(std::move(message));
    offset += 4 + length;
  }

  receiveBuffer.erase(0, offset);
  return messages;
}
