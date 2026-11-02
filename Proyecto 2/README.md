# Maze Craze

Maze Craze is a two-player maze race built with C++17 and Qt. Players start in opposite areas of a generated maze and race to reach the treasure. The maze can contain mud, portals, Shield Tiles, power-up effects, and optional Fog of War.

## Authors

Alejandro, Ashley, and Kevin.

## Features

* **Procedural maze generation** using Depth-First Search (DFS) or Breadth-First Search (BFS).

* **Single-player mode** against an intelligent AI opponent.

* **Local two-player mode** sharing the same computer.

* **Online two-player mode** over TCP connection.

* **Advanced pathfinding algorithms**: BFS, Dijkstra, and A\*.

* **Tactical elements & power-ups**: Mud, portals, Shield Tiles, Jump Wall, Double Play, and Control Enemy.

* **Optional Fog of War** for restricted visibility.

* **Match logging** and integrated tools for algorithm and performance analysis.

## Requirements

* **C++17 compiler** (GCC 9+ or Clang 10+).

* **CMake 3.16** or newer.

* **Qt 5 or Qt 6** with Widgets, Svg, and Network modules.

### Ubuntu / WSL Ubuntu

For **Qt 5**:

```
sudo apt update
sudo apt install build-essential cmake qtbase5-dev libqt5svg5-dev

```

For **Qt 6**:

```
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-svg-dev

```

## Build Instructions

Run the commands from the project directory containing `CMakeLists.txt` (the `Proyecto 2` directory in this repository):

```
cd "Proyecto 2"
cmake -S . -B build
cmake --build build --parallel

```

*(CMake automatically detects an available Qt 5 or Qt 6 installation).*

### Windows (MSVC / MinGW)

Install Qt with the Widgets, Svg, and Network modules, then point CMake to your Qt installation path if it is not detected automatically:

```
cmake -S . -B build -DCMAKE_PREFIX_PATH="<path-to-Qt>"
cmake --build build --config Release --parallel

```

## Running the Application

From the `Proyecto 2` directory:

```
./build/MazeCraze

```

*On Windows, run the generated `MazeCraze.exe` from the build directory or its configuration subdirectory (e.g., `build/Release`).*

The start menu lets you select the game mode, maze generator, and Fog of War option. The online menu also lets you enter the host address and TCP port.

### Controls

| Action | Control | 
| ----- | ----- | 
| **Move** | Arrow keys or `W`, `A`, `S`, `D` | 
| **Jump Wall** | `Shift` + Movement key *(requires a Jump Wall power-up)* | 
| **A* Visualization*\* | Show A\* button *(available when Fog of War is disabled)* | 

## Game Modes

### Single Player

Play against an AI opponent that uses pathfinding algorithms to plan its moves through the maze efficiently.

### Local Two Players

Two players share a single computer. A hand-off screen conceals the board between turns so that the next player cannot view private information.

### Online Multiplayer

Two players compete from separate computers through a TCP connection. One instance acts as the host and the other joins. The host maintains the authoritative game state, and both clients receive the tailored view for their player. Both players must run the identical version of Maze Craze.

#### Host a Match

1. Open Maze Craze and select **Host**.

2. Choose the TCP port (default is `5000`).

3. Start the match and wait for the second player to connect.

#### Join a Match

1. Open Maze Craze on the second computer and select **Join**.

2. Enter the host computer's IP address or hostname.

3. Enter the corresponding TCP port configured by the host.

4. Start the connection to begin the match once both players are linked.

#### Testing Online Mode on a Single Computer

A second physical computer is not required for local testing:

1. Open the first instance of Maze Craze and select **Host** on port `5000`.

2. Open a second instance of Maze Craze.

3. Select **Join**, enter `127.0.0.1` as the address, and use port `5000`.
   *(Note: `127.0.0.1` is the loopback address for local communication only and cannot be used from an external machine).*

### Network and Firewall Requirements

* On the same computer, use `127.0.0.1`.

* On a local network, the joining player must enter the host machine's local IP address.

* The host firewall must permit incoming TCP connections on the selected port (default: `5000`).

* For cross-network connections, port forwarding or router configuration may be necessary.

## Rules and Power-Ups

* Players take turns, with each turn providing the action allowance displayed in the HUD.

* **Mud**: Entering a mud cell consumes additional actions.

* **Portals**: Entering a portal instantly teleports the player to its paired portal, after which the pair is removed.

* **Shield Tile**: Grants a protective barrier against one Control Enemy effect.

* **Fog of War**: Restricts the visible area around the player.

* **Control Enemy**: Forces the opponent to skip their next turn unless blocked by a shield.

* **Double Play**: Grants an extra action during the turn.

* **Jump Wall**: Allows stepping directly across a wall when combined with `Shift`.

*The first player to successfully reach the treasure wins.*

## Match Logs

When a match concludes, the game writes a text log summarizing statistics such as maze-generation metrics, AI searches, turn counts, and power-up or portal utilization.

* Saved directory: `logs/` under the project directory.

* Filename generation handled by `makeLogFilename()` in `src/logging/GameLogger.cpp`.

* In online mode, the host oversees the authoritative state and writes the log.

## Automated Tests

Build the project first, then run all registered automated tests from the `Proyecto 2` directory:

```
ctest --test-dir build --output-on-failure

```

Individual test binaries can also be invoked manually:

```
./build/MazeCrazeTests
./build/IntegrationTests

```

Standalone tests covering Dijkstra, extra passages, and treasure placement are registered directly with CTest. Manual interface checks and online scenarios are detailed in `tests/UiChecklist.md`.

## Analysis Tools

Run these auxiliary tools from the `Proyecto 2` directory:

```
./build/MazeAnalysis
./build/PathComparison
./build/BenchmarkRunner

```

* **MazeAnalysis**: Evaluates and compares maze-generation algorithms.

* **PathComparison**: Compares BFS, Dijkstra, and A\* pathfinding performance.

* **BenchmarkRunner**: Executes automated matches, logging outputs like `data/benchmark_results.csv` and `data/seeds.txt`.
  *(All analysis tools store their output under the `data/` directory).*

## Project Structure

```
Proyecto 2/
├── CMakeLists.txt
├── include/
│   ├── config/
│   ├── core/
│   ├── game/
│   ├── logging/
│   ├── network/
│   └── ui/
├── src/
│   ├── core/
│   ├── game/
│   ├── logging/
│   ├── ui/
│   └── Network/
├── assets/
├── data/
├── logs/
├── tests/
└── tools/

```