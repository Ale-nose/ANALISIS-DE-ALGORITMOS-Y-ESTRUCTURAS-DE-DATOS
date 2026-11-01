// Copyright Alejandro, Ashley and Kevin
#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "GameView.hpp"

struct Action;
struct GameState;

/**
 * @brief Represents the various types of network messages exchanged between
 * client and server.
 */
enum class NetworkMessageType { Hello, Start, Action, State, Error, GameOver };

// Current version of the network communication protocol.
inline constexpr int NETWORK_PROTOCOL_VERSION = 1;
// Maximum allowed size for a single network payload (1 MB).
inline constexpr std::size_t MAX_NETWORK_MESSAGE_SIZE = 1024 * 1024;

/**
 * @brief Serializes a HELLO handshake message.
 * @return JSON-formatted string representing the HELLO message.
 */
std::string serializeHelloMessage();

/**
 * @brief Serializes a START game initiation message.
 * @param playerId Assigned player identifier (0 or 1).
 * @return JSON-formatted string representing the START message.
 */
std::string serializeStartMessage(int playerId);

/**
 * @brief Serializes a player action message.
 * @param action The Action structure containing move or jump details.
 * @return JSON-formatted string representing the ACTION message.
 */
std::string serializeActionMessage(const Action& action);

/**
 * @brief Serializes an error notification message.
 * @param reason Description of the error that occurred.
 * @return JSON-formatted string representing the ERROR message.
 */
std::string serializeErrorMessage(const std::string& reason);

/**
 * @brief Serializes a game over conclusion message.
 * @param winner Index of the winning player (0 or 1).
 * @return JSON-formatted string representing the GAME_OVER message.
 */
std::string serializeGameOverMessage(int winner);

/**
 * @brief Parses raw message data to determine its network message type.
 * @param message Raw string payload received from the socket.
 * @return The corresponding NetworkMessageType enum value.
 */
NetworkMessageType parseNetworkMessageType(const std::string& message);

/**
 * @brief Parses an action message payload into an Action object.
 * @param message Raw JSON string containing the action.
 * @return Populated Action structure.
 */
Action parseActionMessage(const std::string& message);

/**
 * @brief Wraps a network message with a 4-byte big-endian length header
 * (framing).
 * @param message Raw JSON message string.
 * @return Framed byte sequence ready for TCP transmission.
 */
std::string frameNetworkMessage(const std::string& message);

/**
 * @brief Extracts complete delimited messages from the incoming byte stream
 * buffer.
 * @references receiveBuffer Reference to the socket's accumulation buffer.
 * @return Vector of complete, isolated message strings.
 */
std::vector<std::string> extractNetworkMessages(std::string& receiveBuffer);

/**
 * @brief Creates a localized GameView snapshot for a specific client based on
 * Fog of War.
 * @param state Current authoritative GameState.
 * @param clientId Target client identifier (0 or 1).
 * @return Sanitized GameView structure.
 */
GameView createGameView(const GameState& state, int clientId);

/**
 * @brief Serializes a GameView structure into a JSON string.
 * @param view GameView instance to serialize.
 * @return JSON-formatted string of the view.
 */
std::string serializeGameView(const GameView& view);

/**
 * @brief Deserializes a raw JSON message string back into a GameView structure.
 * @param message Raw message payload.
 * @return Deserialized GameView object.
 */
GameView deserializeGameView(const std::string& message);
