// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstdint>

struct Action;

class NetworkServer {
public:
  bool start(uint16_t port);
  void handleAction(int clientId, const Action& action);

private:
  // Server socket, client connections and GameEngine will be added later.
};
