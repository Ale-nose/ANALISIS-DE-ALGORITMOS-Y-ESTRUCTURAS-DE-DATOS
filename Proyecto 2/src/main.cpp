// Copyright Alejandro, Ashley y Kevin
#include <QApplication>
#include <QDebug>

#include "StartWindow.hpp"

// TODO (Issue 34): MainWindow will own StartWindow and call
// GameSetup::createGame(options) when gameStartRequested is emitted.

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  StartWindow startWindow;
  QObject::connect(&startWindow, &StartWindow::gameStartRequested,
    [](const GameOptions& options) {
      qDebug() << "mode:" << static_cast<int>(options.mode)
      << "generator:" << static_cast<int>(options.generator) << "fog:" 
      << options.fogEnabled << "seed:" << options.seed;
    });
  startWindow.show();
  return app.exec();
}
