# Maze Craze

A two-player maze race with Qt5. Both players start in opposite corners of a generated maze and race to the treasure placed between them. The board has mud, portals, shields, power-ups and an optional fog of war.

## Authors
Alejandro, Ashley and Kevin.

## Requirements

- A C++17 compiler (GCC 9+ or Clang 10+)
- CMake 3.16 or newer
- Qt5 with the Widgets, Svg and Network modules

On Ubuntu / WSL Ubuntu:

```bash
sudo apt update
sudo apt install build-essential cmake qtbase5-dev libqt5svg5-dev
```

## Build

All commands are run from the project folder (the one that contains
`CMakeLists.txt`; in our repository it is `"Proyecto 2"`, so quote the name):

```bash
cd "Proyecto 2"
cmake -S . -B build
cmake --build build -j
```

> Windows (MSVC or MinGW with a Qt5 install) uses the same two CMake commands,
> adding `-DCMAKE_PREFIX_PATH=<path to your Qt5 folder>` to the first one.
> Note: The development and primary testing of this project were conducted on Linux/WSL environments.

## Run

```bash
./build/MazeCraze
```

The start window lets you choose the game mode, the maze size, the maze
generator (DFS or BFS), the mud rate and whether fog of war is enabled.

## Controls

| Action | Keys |
|---|---|
| Move | Arrow keys or W, A, S, D |
| Jump through a wall (needs a Jump Wall power-up) | Shift + arrow key or W, A, S, D |
| Show how A* explores the maze (fog disabled only) | "Show A*" button |

## Game Modes

- **Single player**: you play against the AI. The AI plans its moves with
  A* / Dijkstra on the real maze.
- **Local two players**: both players share the same computer. A hand-off
  screen hides the board between turns.
- **Online (host / join)**: TODO

## Rules and Power-Ups

- Every turn a player has one action (a move or a wall jump).
- **Mud**: entering a mud cell costs extra actions, so the player loses turns.
- **Portals**: stepping on a portal moves you to its pair; the pair disappears.
- **Shield tile**: gives a shield that protects against one Control Enemy.
- **Fog of war**: you only see cells within 3 cells (Manhattan distance).
- **Control Enemy**: the rival skips their next turn (unless shielded).
- **Double Play**: gives you one extra action.
- **Jump Wall**: lets you cross one wall with Shift + arrow key.

The first player to reach the treasure wins.

## Logs

When a game ends, a text log with the match statistics (generation metrics,
searches made by the AI, turns played, power-ups and portals used) is written
to disk. The log files are automatically saved in a `logs/` folder at the root of the project (Proyecto 2). The file name is created by `makeLogFilename()` in
`src/logging/GameLogger.cpp`.

## Tests and Tools

```bash
./build/IntegrationTests     # creates a match, plays it to the end and checks the log
./build/MazeCrazeTests       # core unit tests
./build/BenchmarkRunner      # automated matches, writes data/benchmark_results.csv and data/seeds.txt
./build/PathComparison       # compares BFS, Dijkstra and A*
./build/MazeAnalysis         # compares the maze generators
```

The manual interface checklist is in `tests/UiChecklist.md`.

## Project Layout

```
CMakeLists.txt
include/   public headers (core, game, logging, ui, network)
src/       implementation (core, game, logging, ui, network)
assets/    SVG sprites used by the board
tools/     benchmark and analysis programs
tests/     automated tests and the UI checklist
data/      CSV files produced by the tools
```
