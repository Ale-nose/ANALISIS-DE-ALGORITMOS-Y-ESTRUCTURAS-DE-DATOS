// Copyright Alejandro, Ashley and Kevin
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "GameView.hpp"

struct Action;
struct GameState;

enum class NetworkMessageType {
  Hello,
  Start,
  Action,
  State,
  Error,
  GameOver
};

inline constexpr int NETWORK_PROTOCOL_VERSION = 1;
inline constexpr std::size_t MAX_NETWORK_MESSAGE_SIZE = 1024 * 1024;

std::string serializeHelloMessage();
std::string serializeStartMessage(int playerId);
std::string serializeActionMessage(const Action& action);
std::string serializeErrorMessage(const std::string& reason);
std::string serializeGameOverMessage(int winner);

NetworkMessageType parseNetworkMessageType(const std::string& message);
Action parseActionMessage(const std::string& message);

std::string frameNetworkMessage(const std::string& message);
std::vector<std::string> extractNetworkMessages(std::string& receiveBuffer);

GameView createGameView(const GameState& state, int clientId);
std::string serializeGameView(const GameView& view);
