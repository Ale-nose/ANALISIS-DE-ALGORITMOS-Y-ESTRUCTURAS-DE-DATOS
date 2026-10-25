// Copyright Alejandro, Ashley y Kevin
#pragma once

#include "GameTypes.hpp"
#include "GameConfig.hpp"

enum class EventType {
  TURN_CHANGED, 
  PLAYER_MOVED,
  POWER_COLLECTED,
  PORTAL_USED,
  GAME_WON
};

struct GameEvent {
  EventType type = EventType::PLAYER_MOVED; // default event
  Position position{DEFAULT_COORDINATES,DEFAULT_COORDINATES}; // default position of the event
  int player = PLAYER_NONE; // default player 
};
