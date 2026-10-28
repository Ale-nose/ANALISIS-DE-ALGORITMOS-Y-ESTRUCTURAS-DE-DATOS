// Copyright Alejandro, Ashley y Kevin

#include <utility>

#include "GameEngine.hpp"
#include "Visibility.hpp"

GameEngine::GameEngine(GameState gameStateArg): gameState(std::move(gameStateArg))
{ 
}

Position GameEngine::newPos(Position pos, Direction dir) const {
  switch (dir)
  {
  case Direction::UP : {
    Position newPosition {pos.row-1, pos.col};
    return newPosition;
  }
  case Direction::DOWN : {
    Position newPosition {pos.row+1, pos.col};
    return newPosition;
  }
    case Direction::RIGHT : {
    Position newPosition {pos.row, pos.col + 1};
    return newPosition;
  }
    case Direction::LEFT : {
    Position newPosition {pos.row, pos.col - 1};
    return newPosition;
  }
  }
  return pos ;
}

bool GameEngine::submitAction(const Action& action){
  bool result = false;
  switch (action.type)
  {
  case ActionType::MOVE :{
     result = movePlayer(action.direction);
    if (result) {
      this->finishAction();
    }
    return result;

  } case ActionType::JUMP_WALL : {
     result = useJumpWall(action.direction);
     if(result) {
      this->finishAction();
      return result;
     }
  }
}
  return result;
}

bool GameEngine::canMove(Direction directionToCheck) const {
  if (this->gameState.gameOver) {
    return false;
  }

  Position current  = this->gameState.players[this->gameState.currentPlayer].position;
  Position destiny  = newPos(current, directionToCheck);
  return (this->gameState.maze.isInside(destiny) && this->gameState.maze.hasPassage(current,destiny));
}

bool GameEngine::movePlayer(Direction directionToMove) {
  if (!canMove(directionToMove)) {
    return false;
  }
  this->gameState.players[this->gameState.currentPlayer].position = newPos(this->gameState.players
    [this->gameState.currentPlayer].position,directionToMove);
    resolveCellEntry();
  return true;
}

const GameState& GameEngine::state() const {

  return this->gameState;
}

void GameEngine::beginTurn() {
  if(gameState.players[gameState.currentPlayer].skipNextTurn) {
    gameState.players[gameState.currentPlayer].skipNextTurn = false;
    this->advanceTurn();
    return;
  }

   while(gameState.players[gameState.currentPlayer].pendingMudActions > 0 && gameState.actionsLeft > 0) {
    gameState.actionsLeft--;
    gameState.players[gameState.currentPlayer].pendingMudActions --;
  }
  if (gameState.actionsLeft == 0) {
    this->advanceTurn();

  }
}

void GameEngine::finishAction() {
  gameState.actionsLeft--;

  if(gameState.gameOver) {
    gameState.stats.turnsPlayed++;
    gameState.turnCount++;
    return;
  } 

    while(gameState.players[gameState.currentPlayer].pendingMudActions > 0 && gameState.actionsLeft > 0) {
    gameState.actionsLeft--;
    gameState.players[gameState.currentPlayer].pendingMudActions --;
  }

  if(gameState.actionsLeft == 0) {
    gameState.stats.turnsPlayed++;
    gameState.turnCount++;
    GameEngine::advanceTurn();
  }
}

void GameEngine::advanceTurn () {
  gameState.currentPlayer = (gameState.currentPlayer + 1) % NUM_PLAYERS;
  gameState.actionsLeft = ACTIONS_PER_TURN;
  GameEngine::beginTurn();
}

bool GameEngine::useJumpWall(Direction jumpDir) {
  PlayerState& current = gameState.players[currentPlayer()];

  if(gameState.gameOver) {
    return false;
  }
  if(current.jumpWalls <= 0) {
    return false;

  }
  Position destiny = newPos(current.position, jumpDir);
  if (!gameState.maze.isInside(destiny)) {
    return false;
  }

  current.jumpWalls--;
  gameState.stats.powerUpsUsed++;
  current.position = destiny;
  resolveCellEntry();
  return true;
}

void GameEngine::applyControlEnemy() {
  int enemyInd = (gameState.currentPlayer + 1) % NUM_PLAYERS;
  PlayerState& enemy = gameState.players[enemyInd];
  if (enemy.shieldCount > 0) {
    enemy.shieldCount --;
    return;
  }
  enemy.skipNextTurn = true;
}

void GameEngine::applyMudPenalty() {
  PlayerState& current = gameState.players[gameState.currentPlayer];
  current.pendingMudActions += MUD_COST -1; // uses MUD_COST - 1 since enetring mud counts as the first action

}

Position GameEngine::findPairPortal(int portalID, Position origin) const {
  Position positionIt = {DEFAULT_COORDINATES,DEFAULT_COORDINATES};

  for (int i = 0;i < int(gameState.maze.rows()); i++ ) {
    for (int j = 0;j < int(gameState.maze.cols()); j++ ) {
      positionIt.row = i;
      positionIt.col = j;
      if((gameState.maze.cellAt(positionIt).portalID == portalID)&& !(positionIt == origin)) {
        return positionIt;
      }
    }
  }
  return Position{DEFAULT_COORDINATES,DEFAULT_COORDINATES};
}

void GameEngine::activatePortal() {
  PlayerState& current = gameState.players[gameState.currentPlayer];
  if (gameState.maze.cellAt(current.position).portalID == PORTAL_ID) { //default ID for portals when they dont exists
    return;
  }
  Position destiny = findPairPortal(gameState.maze.cellAt(current.position).portalID, current.position);
  if (destiny.col == DEFAULT_COORDINATES || destiny.row == DEFAULT_COORDINATES) { //as long as either is the default
    //theres no valid portal
    return;
  }
  gameState.maze.cellAt(current.position).portalID = PORTAL_ID;
  gameState.maze.cellAt(destiny).portalID = PORTAL_ID;
  current.position = destiny;

  gameState.stats.portalsUsed++;
}

void GameEngine::collectPowerup() {
  PlayerState& current = gameState.players[gameState.currentPlayer];

  switch (gameState.maze.cellAt(current.position).powerUp)
  {
  case  PowerUpType::CONTROL_ENEMY : {
    GameEngine::applyControlEnemy();
    gameState.stats.powerUpsUsed++;
    break;
  }
   case  PowerUpType::DOUBLE_PLAY : {
    GameEngine::doublePlay();
    gameState.stats.powerUpsUsed++;
    break;
  }
   case  PowerUpType::JUMP_WALL : {
    current.jumpWalls++;
    break;
  }
  default:
    break;
  }
  if ( gameState.maze.cellAt(current.position).powerUp != PowerUpType::NONE) {
    gameState.stats.powerUpsCollected++;
  }
  gameState.maze.cellAt(current.position).powerUp = PowerUpType::NONE;
}

void GameEngine::collectShieldTile() {
  PlayerState& current = gameState.players[gameState.currentPlayer];
  if (gameState.maze.cellAt(current.position).special == SpecialCellType::SHIELD_TILE) {
    current.shieldCount++;
    gameState.maze.cellAt(current.position).special = SpecialCellType::NONE;
    
  }
}
void GameEngine::checkWinner() {
  PlayerState& current = gameState.players[gameState.currentPlayer];
  bool isWinner = (current.position == gameState.treasure);
  if (isWinner) {
    gameState.winner = gameState.currentPlayer;
    gameState.gameOver = true;
  }
}

void GameEngine::resolveCellEntry() {
  PlayerState& current = gameState.players[gameState.currentPlayer];
  activatePortal();
  collectPowerup();
  collectShieldTile();
  if (gameState.maze.cellAt(current.position).terrain == TerrainType::MUD) {
    applyMudPenalty();
  }
  checkWinner();
}

std::vector<Position> GameEngine::visibleCells (int playerIndex) const {
  std::vector<Position> cellsVisibleToPlayer;
  for(int i = 0; i < gameState.maze.rows(); i++) {
      for(int j = 0; j < gameState.maze.cols(); j++) {
        Position current = {i,j};
        if(isCellVisible(gameState.players[playerIndex].position,current)) {
          cellsVisibleToPlayer.push_back(current);
        }
      }
    }
  return cellsVisibleToPlayer;
}

void GameEngine::recordAiSearch(const SearchRecord& record) {
  this->gameState.stats.searches.push_back(record);
  this->gameState.stats.aiReplans.push_back(record.reason);
}
