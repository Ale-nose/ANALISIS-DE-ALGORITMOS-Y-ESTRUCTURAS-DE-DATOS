// Copyright Alejandro, Ashley y Kevin
#include <QApplication>
#include <QMainWindow>
 
// TODO (Issue 34): replace the temporary window with
// #include "mazecraze/ui/MainWindow.h" and mc::MainWindow.
 
int main(int argc, char* argv[]) {
  QApplication app(argc, argv);

  // Temporary empty window until MainWindow is implemented.
  QMainWindow window;
  window.setWindowTitle("Maze Craze");
  window.resize(800, 600);
  window.show();

  return app.exec();
}
