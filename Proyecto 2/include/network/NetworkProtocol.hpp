// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <string>

#include "GameView.hpp"

struct Action;
struct GameState;

GameView createGameView(const GameState& state, int clientId);
std::string serializeGameView(const GameView& view);
Action parseActionMessage(const std::string& message);
