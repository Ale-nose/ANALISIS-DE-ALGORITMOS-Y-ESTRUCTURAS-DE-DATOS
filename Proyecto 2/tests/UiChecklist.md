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
| 1 | Open the program | The start menu appears with the dark theme and the title | The start menu opened with the dark theme, the title and all the groups (game mode, connection, maze generation, fog). | Pass |
| 2 | Menu: mode options | Single player, Local two players, Host and Join can be selected, one at a time | Four modes shown; selecting one deselects the previous one. | Pass |
| 3 | Menu: generator | DFS and BFS can be selected; DFS is the default | DFS and BFS are available; DFS is selected by default. | Pass |
| 4 | Menu: Join selected | Address and port are enabled; generator and Fog are disabled | With Join: address and port can be edited; generator and Fog are disabled (DFS stays marked). Host enables connection, generator and Fog; Single and Two Players enable only generator and Fog. | Pass |
| 5 | START, single player, DFS | The board and the HUD appear; Player 1 starts | Board and HUD appeared and Player 1 started. | Pass |
| 6 | START, single player, BFS | A different maze is drawn; the game starts | A clearly different maze was drawn (long horizontal corridors) and the game started. | Pass |
| 7 | START, local two players | The board and the HUD appear | Board and HUD appeared. | Pass |
| 8 | Board images | Players, treasure, portals (with readable number), Shield Tiles, power-ups and mud are drawn with their images | Players, treasure, portals with readable numbers, Shield Tiles, power-ups and mud are drawn with their images. | Pass |
| 9 | Move with arrows | The active player moves one cell if there is no wall | The active player moves one cell when there is no wall. | Pass |
| 10 | Move with WASD | Same as the arrows | Same behaviour as the arrows. | Pass |
| 11 | Move against a wall | Nothing happens and the turn is not lost | Nothing happened and the turn was not lost. | Pass |
| 12 | Jump Wall (modifier + direction) | It jumps the wall only when the player has a Jump Wall; the HUD count goes down | Jumps the wall only with a Jump Wall; the HUD count goes down; without one nothing happens. | Pass |
| 13 | HUD | It shows the turn, actions, Jump Walls and shields of the active player | HUD shows Turn (Player 1/2), Actions left, Jump Walls and Shields. | Pass |
| 14 | Mud | Entering mud makes the player lose the following turn(s) | Entering mud made the player lose the next turn. | Pass |
| 15 | Portal | Entering a portal moves the player to the paired portal, once | The player appeared at the portal with the same number; the pair disappeared after one use. | Pass |
| 16 | Shield Tile | The player gets the aura image and the HUD shield count goes up | The player shows the aura and the HUD shield count goes up. | Pass |
| 17 | Control Enemy | The rival loses the next turn, unless a shield absorbs it | The rival lost the next turn; with a shield the shield was spent and the turn was kept. | Pass |
| 18 | Fog of War on | Only cells within 3 steps are visible; the rival shows only when close | Only cells within 3 steps are visible; the rival appears only when close. | Pass |
| 19 | Fog of War off | The whole maze is visible | The whole maze is visible. | Pass |
| 20 | Local: turn handoff | After a move the result is visible for a moment, then the handoff screen covers the board; Ready shows the next player's view | The result is visible for a moment, then the handoff screen covers the board; Ready shows the next player's view. | Pass |
| 21 | Local: no leak | The handoff screen hides the board and the HUD counts | Before pressing Ready the screen hides both the board and the HUD counts. | Pass |
| 22 | Single player: AI | The AI moves by itself with a pause and the window never freezes | The AI moves by itself with a pause and the window never freezes. | Pass |
| 23 | Keys during the AI turn | They are ignored | Keys are ignored during the AI turn (the AI moves fast, so it is hard to time). | Pass |
| 24 | Maze size | The whole maze fits on the screen; cells are never smaller than the minimum | The whole maze fits, also when maximized; cells never go below the minimum size. | Pass |
| 25 | Resize the window | The board is redrawn centered with square cells | Board is redrawn centered with square cells. With a very small window the board is hard to see. | Pass |
| 26 | Reach the treasure (local) | The game over dialog names the winner | The dialog names the winner and offers Quit and New game; the log file is created. | Pass |
| 27 | Reach the treasure (single player) | The dialog says whether the user or the AI won | The dialog says whether the user or the AI won and offers Quit and New game; the logs folder is created automatically. | Pass |
| 28 | Game log | `logs/mazecraze_<date>_<time>.txt` exists with all sections and the same winner as the dialog | logs/mazecraze_<date>_<time>.txt is created when the match ends with all sections; the winner matches the dialog (checked on a sample log). | Pass |
| 29 | Two matches in a row | Two different log files; the first one is not overwritten | After New game and another match a second, different log file was created. | Pass |
| 30 | Log cannot be written (make `logs` read-only) | A warning with the file name appears | With logs read-only a warning says the game log could not be saved; after chmod u+w the next match saved its log. | Pass |
| 31 | Dialog: New game | Back to the menu; a new match can be started | Back to the menu and a new match can be started. | Pass |
| 32 | Dialog: Quit | The program closes | The program closes. | Pass |
| 33 | Esc on the dialogs | It does not close them | Esc does not close the dialogs. | Pass |
| 34 | Online (when connected): host + join | Both windows show the same match and update after every move | Host (port 5000) and Join (127.0.0.1:5000) in two windows show the same match and update after every move. | Pass |
| 35 | Show A* (fog off) | The exploration is animated without freezing the window and the final path is drawn in orange; the button toggles it off | The animation played and the orange path stayed at the end. | Pass |
| 36 | Show A* (fog on) | The button does not appear | The button is not shown with fog enabled. | Pass |

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