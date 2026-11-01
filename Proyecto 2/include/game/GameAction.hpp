// Copyright Alejandro, Ashley y Kevin
#pragma once

#include "GameTypes.hpp"

struct Action {
  ActionType type = ActionType::MOVE;     // default actions to take
  Direction direction = Direction::DOWN;  // default direction to take
};
