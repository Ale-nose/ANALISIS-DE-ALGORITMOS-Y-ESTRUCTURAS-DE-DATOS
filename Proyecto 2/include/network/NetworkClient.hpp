// Copyright Alejandro, Ashley y Kevin
#pragma once

#include <cstdint>
#include <string>

struct Action;

class NetworkClient {
public:
  bool connectTo(const std::string& host, uint16_t port);
  bool sendAction(const Action& action);

private:
  // TCP socket and received GameView will be added later.
};
