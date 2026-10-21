// Copyright Alejandro, Ashley and Kevin
#include "NetworkProtocol.hpp"

#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <utility>

#include <QByteArray>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QString>

#include "GameAction.hpp"
#include "GameConfig.hpp"
#include "GameState.hpp"

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
    case ActionType::MOVE:
      kind = QStringLiteral("MOVE");
      break;
    case ActionType::JUMP_WALL:
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
    action.type = ActionType::MOVE;
  } else if (kind == QStringLiteral("JUMP_WALL")) {
    action.type = ActionType::JUMP_WALL;
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

GameView createGameView(const GameState& state, int clientId) {
  if (clientId != PLAYER_1 && clientId != PLAYER_2) {
    throw std::invalid_argument("Invalid client ID");
  }

  const Position viewer = state.players[clientId].position;
  if (state.options.fogEnabled && !state.maze.isInside(viewer)) {
    throw std::invalid_argument("Viewer position is outside the maze");
  }

  GameView view{};
  view.rows = state.maze.rows();
  view.cols = state.maze.cols();

  const Position treasure = state.treasure;

  if (state.maze.isInside(treasure)) {
    const int distance = std::abs(viewer.row - treasure.row) + std::abs(viewer.col - treasure.col);
    if (!state.options.fogEnabled || distance <= FOG_RADIUS) {
      view.treasure = treasure;
    }
  }

  view.currentPlayer = state.currentPlayer;
  view.gameOver = state.gameOver;
  view.winner = state.winner;

  for (int row = 0; row < state.maze.rows(); ++row) {
    for (int col = 0; col < state.maze.cols(); ++col) {
      const Position position{row, col};
      const int distance = std::abs(viewer.row - row) + std::abs(viewer.col - col);
      if (!state.options.fogEnabled || distance <= FOG_RADIUS) {
        view.visibleCells.push_back({position, state.maze.cellAt(position)});
      }
    }
  }

  for (int playerId = 0; playerId < NUM_PLAYERS; ++playerId) {
    const Position position = state.players[playerId].position;
    const int distance = std::abs(viewer.row - position.row) + std::abs(viewer.col - position.col);
    const bool visible = state.maze.isInside(position)
      && (!state.options.fogEnabled || playerId == clientId || distance <= FOG_RADIUS);
    view.players[playerId].visible = visible;
    view.players[playerId].position = visible ? position : Position{-1, -1};
  }

  return view;
}

static QJsonObject positionToJson(Position position) {
  QJsonObject result;
  result["row"] = position.row;
  result["col"] = position.col;
  return result;
}

static QJsonObject cellToJson(const Cell& cell) {
  QJsonArray walls;
  for (bool closed : cell.walls) {
    walls.append(closed);
  }

  QJsonObject result;
  result["terrain"] = static_cast<int>(cell.terrain);
  result["powerUp"] = static_cast<int>(cell.powerUp);
  result["special"] = static_cast<int>(cell.special);
  result["portalID"] = cell.portalID;
  result["walls"] = walls;
  return result;
}

std::string serializeGameView(const GameView& view) {
  QJsonArray visibleCells;
  for (const VisibleCell& cell : view.visibleCells) {
    QJsonObject item;
    item["position"] = positionToJson(cell.position);
    item["contents"] = cellToJson(cell.contents);
    visibleCells.append(item);
  }

  QJsonArray players;
  for (const PlayerPublicView& player : view.players) {
    QJsonObject item;
    item["visible"] = player.visible;
    if (player.visible) {
      item["position"] = positionToJson(player.position);
    }
    players.append(item);
  }

  QJsonObject gameView;
  gameView["rows"] = view.rows;
  gameView["cols"] = view.cols;

  if (view.treasure.row >= 0 && view.treasure.col >= 0) {
    gameView["treasure"] = positionToJson(view.treasure);
  }

  gameView["visibleCells"] = visibleCells;
  gameView["players"] = players;
  gameView["currentPlayer"] = view.currentPlayer;
  gameView["gameOver"] = view.gameOver;
  gameView["winner"] = view.winner;

  QJsonObject message;
  message["type"] = QStringLiteral("STATE");
  message["view"] = gameView;

  std::string json = writeJsonObject(message);
  if (json.size() > MAX_NETWORK_MESSAGE_SIZE) {
    throw std::length_error("GameView message exceeds the maximum size");
  }
  return json;
}

static Position positionFromJson(const QJsonValue& value) {
  if (!value.isObject()) {
    throw std::invalid_argument("Invalid position");
  }

  const QJsonObject object = value.toObject();

  if (!object.value("row").isDouble() || !object.value("col").isDouble()) {
    throw std::invalid_argument("Invalid coordinates");
  }

  return Position{object.value("row").toInt(-1), object.value("col").toInt(-1)};
}

GameView deserializeGameView(const std::string& message) {
  if (parseNetworkMessageType(message) != NetworkMessageType::State) {
    throw std::invalid_argument("Expected STATE message");
  }

  const QJsonObject root = readJsonObject(message);

  if (!root.value("view").isObject()) {
    throw std::invalid_argument("Missing GameView");
  }

  const QJsonObject data = root.value("view").toObject();

  if (!data.value("rows").isDouble() || !data.value("cols").isDouble()) {
    throw std::invalid_argument("Missing maze dimensions");
  }

  GameView view;
  view.rows = data.value("rows").toInt();
  view.cols = data.value("cols").toInt();

  if (view.rows <= 0 || view.cols <= 0 || view.rows > 10000 || view.cols > 10000 || view.rows * view.cols > 10000) {
    throw std::invalid_argument("Invalid maze dimensions");
  }

  if (!data.value("currentPlayer").isDouble() || !data.value("gameOver").isBool() || !data.value("winner").isDouble()) {
    throw std::invalid_argument("Invalid match information");
  }

  view.currentPlayer = data.value("currentPlayer").toInt(-1);
  view.gameOver = data.value("gameOver").toBool();
  view.winner = data.value("winner").toInt(-1);

  if (view.currentPlayer < 0 || view.currentPlayer >= NUM_PLAYERS || view.winner < -1 || view.winner >= NUM_PLAYERS) {
    throw std::invalid_argument("Invalid player information");
  }

  if (data.contains("treasure")) {
    view.treasure = positionFromJson(data.value("treasure"));

    if (view.treasure.row < 0 || view.treasure.row >= view.rows ||
      view.treasure.col < 0 || view.treasure.col >= view.cols) {
      throw std::invalid_argument("Invalid treasure position");
    }
  }

  if (!data.value("visibleCells").isArray()) {
    throw std::invalid_argument("Missing visible cells");
  }

  const QJsonArray cells = data.value("visibleCells").toArray();

  if (cells.size() > view.rows * view.cols) {
    throw std::invalid_argument("Too many visible cells");
  }

  std::vector<bool> received(
    static_cast<std::size_t>(view.rows) * view.cols, false
  );

  for (const QJsonValue& entry : cells) {
    if (!entry.isObject()) {
      throw std::invalid_argument("Invalid visible cell");
    }

    const QJsonObject item = entry.toObject();
    const Position position = positionFromJson(item.value("position"));

    if (position.row < 0 || position.row >= view.rows || position.col < 0 || position.col >= view.cols
      || !item.value("contents").isObject()) {
      throw std::invalid_argument("Invalid cell data");
    }

    const std::size_t index = static_cast<std::size_t>(position.row) * view.cols + position.col;

    if (received[index]) {
      throw std::invalid_argument("Duplicate visible cell");
    }

    received[index] = true;

    const QJsonObject contents = item.value("contents").toObject();

    if (!contents.value("terrain").isDouble() || !contents.value("powerUp").isDouble()||
      !contents.value("special").isDouble() || !contents.value("portalID").isDouble() ||
      !contents.value("walls").isArray()) {
      throw std::invalid_argument("Invalid cell contents");
    }

    const int terrain = contents.value("terrain").toInt(-1);
    const int powerUp = contents.value("powerUp").toInt(-1);
    const int special = contents.value("special").toInt(-1);

    if (terrain < 0 || terrain > static_cast<int>(TerrainType::MUD) ||
      powerUp < 0 || powerUp > static_cast<int>(PowerUpType::CONTROL_ENEMY) ||
      special < 0 || special > static_cast<int>(SpecialCellType::SHIELD_TILE)) {
      throw std::invalid_argument("Unknown cell type");
    }

    Cell cell;
    cell.terrain = static_cast<TerrainType>(terrain);
    cell.powerUp = static_cast<PowerUpType>(powerUp);
    cell.special = static_cast<SpecialCellType>(special);
    cell.portalID = contents.value("portalID").toInt(-1);

    const QJsonArray walls = contents.value("walls").toArray();

    if (walls.size() != 4) {
      throw std::invalid_argument("Invalid walls");
    }

    for (int wall = 0; wall < 4; ++wall) {
      if (!walls.at(wall).isBool()) {
        throw std::invalid_argument("Invalid wall value");
      }
      cell.walls[static_cast<std::size_t>(wall)] = walls.at(wall).toBool();
    }

    view.visibleCells.push_back({position, cell});
  }

  if (view.treasure.row >= 0) {
    const std::size_t index = static_cast<std::size_t>(view.treasure.row) * view.cols + view.treasure.col;
    if (!received[index]) {
      throw std::invalid_argument("Treasure is outside visible cells");
    }
  }

  if (!data.value("players").isArray()) {
    throw std::invalid_argument("Missing players");
  }

  const QJsonArray players = data.value("players").toArray();

  if (players.size() != NUM_PLAYERS) {
    throw std::invalid_argument("Invalid player count");
  }

  for (int id = 0; id < NUM_PLAYERS; ++id) {
    if (!players.at(id).isObject()) {
      throw std::invalid_argument("Invalid player");
    }

    const QJsonObject player = players.at(id).toObject();

    if (!player.value("visible").isBool()) {
      throw std::invalid_argument("Invalid player visibility");
    }

    view.players[id].visible = player.value("visible").toBool();

    if (view.players[id].visible) {
      const Position position = positionFromJson(player.value("position"));

      if (position.row < 0 || position.row >= view.rows || position.col < 0 || position.col >= view.cols) {
        throw std::invalid_argument("Invalid player position");
      }

      const std::size_t index = static_cast<std::size_t>(position.row) * view.cols + position.col;

      if (!received[index]) {
        throw std::invalid_argument("Player outside visible cells");
      }

      view.players[id].position = position;
    } else if (player.contains("position")) {
      throw std::invalid_argument("Hidden player position exposed");
    }
  }

  return view;
}
