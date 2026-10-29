# UI checklist

Manual test of the whole game in Qt, from the menu to the victory screen.
Run it on the final build: `cmake --build build --target MazeCraze && ./build/MazeCraze`.

How to fill it: write what really happened in **Observed** and `Pass` or `Fail`
in the last column. A case that failed gets a row in "Errors found and fixed".

Automatic checks (no window) are in `tests/IntegrationTests.cpp`
(`./build/IntegrationTests`).

## Cases

| # | Case | Expected | Observed | Pass/Fail |
|---|------|----------|----------|-----------|
| 1 | Open the program | The start menu appears with the dark theme and the title | | |
| 2 | Menu: mode options | Single player, Local two players, Host and Join can be selected, one at a time | | |
| 3 | Menu: generator | DFS and BFS can be selected; DFS is the default | | |
| 4 | Menu: Join selected | Address and port are enabled; generator and Fog are disabled | | |
| 5 | START, single player, DFS | The board and the HUD appear; Player 1 starts | | |
| 6 | START, single player, BFS | A different maze is drawn; the game starts | | |
| 7 | START, local two players | The board and the HUD appear | | |
| 8 | Board images | Players, treasure, portals (with readable number), Shield Tiles, power-ups and mud are drawn with their images | | |
| 9 | Move with arrows | The active player moves one cell if there is no wall | | |
| 10 | Move with WASD | Same as the arrows | | |
| 11 | Move against a wall | Nothing happens and the turn is not lost | | |
| 12 | Jump Wall (modifier + direction) | It jumps the wall only when the player has a Jump Wall; the HUD count goes down | | |
| 13 | HUD | It shows the turn, actions, Jump Walls and shields of the active player | | |
| 14 | Mud | Entering mud makes the player lose the following turn(s) | | |
| 15 | Portal | Entering a portal moves the player to the paired portal, once | | |
| 16 | Shield Tile | The player gets the aura image and the HUD shield count goes up | | |
| 17 | Control Enemy | The rival loses the next turn, unless a shield absorbs it | | |
| 18 | Fog of War on | Only cells within 3 steps are visible; the rival shows only when close | | |
| 19 | Fog of War off | The whole maze is visible | | |
| 20 | Local: turn handoff | After a move the result is visible for a moment, then the handoff screen covers the board; Ready shows the next player's view | | |
| 21 | Local: no leak | The handoff screen hides the board and the HUD counts | | |
| 22 | Single player: AI | The AI moves by itself with a pause and the window never freezes | | |
| 23 | Keys during the AI turn | They are ignored | | |
| 24 | Maze size | The whole maze fits on the screen; cells are never smaller than the minimum | | |
| 25 | Resize the window | The board is redrawn centered with square cells | | |
| 26 | Reach the treasure (local) | The game over dialog names the winner | | |
| 27 | Reach the treasure (single player) | The dialog says whether the user or the AI won | | |
| 28 | Game log | `logs/mazecraze_<date>_<time>.txt` exists with all sections and the same winner as the dialog | | |
| 29 | Two matches in a row | Two different log files; the first one is not overwritten | | |
| 30 | Log cannot be written (make `logs` read-only) | A warning with the file name appears | | |
| 31 | Dialog: New game | Back to the menu; a new match can be started | | |
| 32 | Dialog: Quit | The program closes | | |
| 33 | Esc on the dialogs | It does not close them | | |
| 34 | Online (when connected): host + join | Both windows show the same match and update after every move | | |

## Errors found and fixed

| Error | Cause | Fix |
|-------|-------|-----|
| `QStackedWidget::setCurrentWidget: widget not contained in stack` | The match page was never added to the stack | `pages_->addWidget(gamePage_)` |
| The keys did nothing after adding the HUD | `keyPressEvent` compared with the board instead of the match page | Compare with `gamePage_` |
| The menu appeared gray inside the main window | The style background was not painted | `Qt::WA_StyledBackground` in `StartWindow` |
| The AI never moved (build error `runAiTurn`) | The method was missing in the header | Declared `runAiTurn` in `MainWindow.hpp` |
| Portal numbers were hard to read | White text over the portal spiral | Dark badge behind the number and a bigger font |
| In local mode the handoff screen appeared before the move was seen | The window was covered right after the action | The move is drawn, a short pause, then the handoff screen |
| The game log had no AI searches | The window did not hand the AI search to the engine | `takeLatestSearchRecord` + `recordAiSearch` in `runAiTurn` |
| The game log showed 0 portals and power-ups used | The engine never counted them | Counters in `activatePortal`, `collectPowerup` and `useJumpWall` |
