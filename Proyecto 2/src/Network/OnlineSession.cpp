// Copyright Alejandro, Ashley y Kevin
#include "OnlineSession.hpp"

#include <QObject>
#include <QString>
#include <memory>
#include <stdexcept>
#include <utility>

#include "GameEngine.hpp"
#include "GameSetup.hpp"
#include "NetworkClient.hpp"
#include "NetworkServer.hpp"

struct OnlineSession::Private {
  std::unique_ptr<GameEngine> engine;
  std::unique_ptr<NetworkServer> server;
  std::unique_ptr<NetworkClient> client;
  bool started = false;
  bool closing = false;

  void connectSignals(OnlineSession* session) {
    QObject::connect(client.get(), &NetworkClient::startReceived, session,
                     [this, session](int playerId) {
                       started = true;
                       emit session->playerAssigned(playerId);
                     });

    QObject::connect(
        client.get(), &NetworkClient::gameViewReceived, session,
        [session](const GameView& view) { emit session->viewReceived(view); });

    QObject::connect(
        client.get(), &NetworkClient::gameOverReceived, session,
        [session](int winner) { emit session->matchEnded(winner); });

    QObject::connect(client.get(), &NetworkClient::errorReceived, session,
                     [this, session](const QString& error) {
                       if (started) {
                         emit session->notice(error);
                       } else {
                         emit session->errorOccurred(error);
                       }
                     });

    QObject::connect(client.get(), &NetworkClient::connectionClosed, session,
                     [this, session]() {
                       if (!closing) {
                         emit session->connectionClosed();
                       }
                     });
  }

  void reset() {
    closing = true;

    if (client) {
      client->disconnectFromServer();
    }

    client.reset();
    server.reset();
    engine.reset();
    started = false;
    closing = false;
  }
};

OnlineSession::OnlineSession(QObject* parent)
    : QObject(parent), d_(std::make_unique<Private>()) {}

OnlineSession::~OnlineSession() = default;

bool OnlineSession::host(const GameOptions& options, uint16_t port) {
  close();

  if (port == 0) {
    return false;
  }

  try {
    GameOptions onlineOptions = options;
    onlineOptions.mode = GameMode::ONLINE;

    d_->engine =
        std::make_unique<GameEngine>(GameSetup::createGame(onlineOptions));
    d_->server = std::make_unique<NetworkServer>(*d_->engine);

    if (!d_->server->start(port)) {
      d_->reset();
      return false;
    }

    d_->client = std::make_unique<NetworkClient>();
    d_->connectSignals(this);

    if (!d_->client->connectTo("127.0.0.1", port)) {
      d_->reset();
      return false;
    }
  } catch (const std::exception&) {
    d_->reset();
    return false;
  }

  return true;
}

bool OnlineSession::join(const std::string& address, uint16_t port) {
  close();

  if (address.empty() || port == 0) {
    return false;
  }

  d_->client = std::make_unique<NetworkClient>();
  d_->connectSignals(this);

  if (!d_->client->connectTo(address, port)) {
    d_->reset();
    return false;
  }

  return true;
}

bool OnlineSession::sendAction(const Action& action) {
  return d_->started && d_->client && d_->client->sendAction(action);
}

void OnlineSession::close() { d_->reset(); }

const GameState* OnlineSession::authoritativeState() const {
  return d_->engine ? &d_->engine->state() : nullptr;
}
