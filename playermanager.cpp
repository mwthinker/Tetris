#include "playermanager.h"

#include "remoteuser.h"
#include "human.h"

#include <string>
#include <vector>
#include <sstream>

#include <mw/servermanager.h>
#include <mw/clientmanager.h>
#include <mw/localmanager.h>

// public
PlayerManager::PlayerManager() {
	status_ = WAITING_TO_CONNECT;
	start_ = false;
	serverPort_ = 12008;
	connectToPort_ = 12008;
	connectToIp_ = "localhost";
	manager_ = 0;
	ready_ = false;
	
	nbrOfAlivePlayers_ = 0;
	acceptNewConnections_ = false;
}

PlayerManager::~PlayerManager() {
	stop();
	delete manager_;
}

int PlayerManager::getNumberOfConnections() const {
	return 1 + remoteUsers_.size();
}

int PlayerManager::getNumberOfPlayers(int connection) const {
	if (connection == 0) {
		return humans_.size();
	}
	return remoteUsers_[connection-1]->getNbrOfPlayers();
}

// Initiates the choosen connection.
void PlayerManager::connect(const std::vector<HumanPtr>& humans, Status status) {
	if (status_ == WAITING_TO_CONNECT) {
		humans_.clear(); // Clear old data.			
		for (const HumanPtr& human : humans) {
			humans_.push_back(PairHumanIndex(human,-1));
		}
		delete manager_;
		status_ = status;
		acceptNewConnections_ = true;

		switch (status) {
		case LOCAL:
			manager_ = new mw::LocalManager(this);
			manager_->start();
			std::cout << "\nLocal" << std::endl;
			// Add new player to all human players.
			for (PairHumanIndex& pair : humans_) {
				pair.second = players_.size();
				players_.push_back(new Player(++playerId_));
			}
			break;
		case SERVER:
			manager_ = new mw::ServerManager(serverPort_,this);
			manager_->start();
			// Add new player to all human players.
			for (PairHumanIndex& pair : humans_) {
				pair.second = players_.size();
				players_.push_back(new Player(++playerId_));
			}
			break;
		case CLIENT:
			manager_ = new mw::ClientManager(connectToPort_,connectToIp_);
			manager_->start();
			sendClientInfo();
			break;
		};
	}
}

// Returns true if start() has been called. Returns False
// when stop() has been called.
bool PlayerManager::isStarted() {
	return start_;
}

void PlayerManager::setReady(bool ready) {
	if (manager_ != 0 && !isStarted()) {
		ready_ = ready;
		sendReady();
	}
}

void PlayerManager::restart() {
	// Manager is active and is server and game is started => restart,
	if (manager_ != 0 && isStarted() && manager_->getId() == manager_->getServerId()) {
		serverSendStartGame();
	}
}

bool PlayerManager::isReady() const {
	return ready_;
}

void PlayerManager::setConnectToPort(int port) {
	connectToPort_ = port;
}

int PlayerManager::getConnectToPort() const {
	return connectToPort_;
}

void PlayerManager::setServerPort(int port) {
	serverPort_ = port;
}

int PlayerManager::getServerPort() const {
	return serverPort_;
}

void PlayerManager::setConnectToIp(std::string ip) {
	connectToIp_ = ip;
}

std::string PlayerManager::getConnectToIp() const {
	return connectToIp_;
}

// Starts the game. Must be ready to start.
void PlayerManager::start() {
	// Game not started. // Connection must be active!
	if (manager_ != 0 && !start_ && manager_->getStatus() == mw::ConnectionManager::ACTIVE) {		
		// Is server.
		if (manager_->getId() == manager_->getServerId()) {
			if (!isReady()) {
				return;
			}
			std::cout << "\nReady!" << std::endl;

			// Stops new connections.
			acceptNewConnections_ = false;

			// Check if all remote users are ready to start.
			for (RemoteUser* remoteUser : remoteUsers_) {
				if (!remoteUser->isReady()) {
					// Not ready -> dont start!
					return;
				}
			}
			std::cout << "\nReady2!" << std::endl;

			serverSendStartGame();
		} else { // Is client.
		}
	}		
}

// Stops the game and aborts any active connection.
void PlayerManager::stop() {
	if (manager_ != 0) {
		//players_.clear();
		start_ = false;
		ready_ = false;
		pause_ = false;
		status_ = WAITING_TO_CONNECT;

		// Disconnecting
		manager_->stop();

		for (RemoteUser* remoteUser : remoteUsers_) {
			delete remoteUser;
		}
		remoteUsers_.clear();
		for (Player* player : players_) {
			delete player;
		}
		players_.clear();
		humans_.clear();
	}
}



void PlayerManager::update(Uint32 deltaTime) {
	if (manager_ != 0) {
		manager_->update();
		switch (manager_->getStatus()) {
		case mw::ConnectionManager::NOT_ACTIVE:
			//std::cout << "\nNOT_ACTIVE: " << std::endl;
			break;
		case mw::ConnectionManager::ACTIVE:
			{				
				mw::Packet data;
				int id = 0;
				while (id = manager_->pullFromReceiveBuffer(data)) {
					receiveData(data, id);
				}

				if (isStarted()) {
					for (auto it = humans_.begin(); it != humans_.end(); ++it) {
						HumanPtr& human = it->first;
						int index = it->second;
						Player* player = players_[index];
						int level = player->getLevel();

						if (!isPaused()) {
							human->update(deltaTime/1000.0,level);
						}						

						TetrisBoard::Move move;
						while (human->pollMove(move)) {
							sendInput(player->getId(),move,player->getTetrisBoard().nextBlock().blockType());
						}
					}
				}
			}
			break;
		case mw::ConnectionManager::DISCONNECTING:
			// Will soon change status to not active.
			std::cout << "\nDISCONNECTING: " << std::endl;
			break;
		}
	} else {
		status_ = WAITING_TO_CONNECT;
	}
}

PlayerManager::Status PlayerManager::getStatus() const {
	return status_;
}

void PlayerManager::applyRules(Player* player, GameEvent gameEvent) {
	// Warning a slight rick to be out of sync in multiplayer.
	// however only effecting points and level and in very subtle ways.
	// Nothing other than graphics is effected.
	int rows = 0;
	switch (gameEvent) {
	case ONE_ROW_REMOVED:
		rows = 1;
		break;
	case TWO_ROW_REMOVED:
		rows = 2;
		break;
	case THREE_ROW_REMOVED:
		rows = 3;
		break;
	case FOUR_ROW_REMOVED:
		{
			rows = 4;

			// More than one player->
			if (players_.size() > 1) {
				for (auto it = humans_.begin(); it != humans_.end(); ++it) {
					// Human player => sendTetrisInfo() to all players.
					int nbrOfPlayers = players_.size();
					int nbrOfRowsToAdd = 4;
					if (nbrOfPlayers == 2) {
						nbrOfRowsToAdd = 4;
					} else if (nbrOfPlayers == 3) {
						nbrOfRowsToAdd = 2;
					} else {
						nbrOfRowsToAdd = 1;
					}

					// Send rows to all humans player except to the player who
					// triggered the event.
					Player* tmpPlayer = players_[it->second];
					if (player != tmpPlayer) {
						std::vector<BlockType> blockTypes;						
						for (int i = 0; i < nbrOfRowsToAdd; ++i) {
							std::vector<BlockType> tmp = tmpPlayer->getTetrisBoard().generateRow();
							blockTypes.insert(blockTypes.begin(),tmp.begin(),tmp.end());
						}
						sendTetrisInfo(tmpPlayer->getId(), blockTypes);
					}
				}
			}
			break;
		}
	case GAME_OVER:
		// Multiplayer?
		if (players_.size() > 1) {			
			std::stringstream stream;
			stream << nbrOfAlivePlayers_ ;
			if (nbrOfAlivePlayers_ == 1) {
				stream << ":st place!";
			} else if (nbrOfAlivePlayers_ == 2) {
				stream << ":nd place!";
			} else if (nbrOfAlivePlayers_ == 3) {
				stream << ":rd place!";
			} else {
				stream << ":th place!";
			}
			
			player->setGameOverMessage(stream.str());
			
			// One player more is dead.
			--nbrOfAlivePlayers_;

			// All dead except one => End game!
			if (nbrOfAlivePlayers_ == 1) {
				for (Player* tmp : players_) {
					// Will be noticed in the next call to PlayerManager::applyRules(...).
					// Triggers only for not dead players.
					tmp->tetrisBoard_.triggerGameOverEvent();
				}
			}
		} else {
			// Singleplayer.
			player->setGameOverMessage("Game over!");
		}
		break;
	default:
		break;
	}	

	if (rows != 0) {
		// Assign points and number of cleared rows.
		player->setNbrOfClearedRows(player->getNbrOfClearedRows() + rows);
		player->setPoints(player->getPoints() + player->getLevel()*rows*rows);

		int nbrOfRowsToLevelUp = 10;

		// Increase level up counter.
		// If assuming all players are equally good. Players should level up with the 
		// the same time in both single- and multiplayer game.
		if (players_.size() > 1) {
			// Multiplayer

			// Increase level up counter for all opponents to the current player.
			for (Player* opponent : players_) {
				if (opponent != player) {
					// Compensates for more opponents which are also increasing counter
					// Compared to singleplayer.
					int add = rows*(players_.size() - nbrOfAlivePlayers_ + 1);
					opponent->setLevelUpCounter(opponent->getLevelUpCounter() + add);
				}
			}			
		} else {
			// Singleplayer.
			player->setLevelUpCounter(player->getLevelUpCounter() + rows);			
		}

		// Set level to this player. Only when this players cleares a row.
		int maxLevel = 20;
		if (players_.size() > 1) {
			// Multiplayer
			// Higher counter bar to level up due to more players that contribute to 
			// increase counter.
			int level = player->getLevelUpCounter() / (nbrOfRowsToLevelUp*(players_.size()-1)) + 1;
			if (level <= maxLevel) {				
				player->setLevel(level);
			}
		} else {
			int level = (player->getLevelUpCounter() / nbrOfRowsToLevelUp) + 1;
			if (level <= maxLevel) {
				player->setLevel(level);
			}
		}
	}
}

/*
// Update to new human players. Will return true if the game has 
// not started and is not set to ready.
// Will else return false.
// A call to start() has to been called before else it will fail 
// and return false.
bool updateAddNewHumans(const std::vector<HumanPtr>& humans) {
// Not ready and not started and a valid manger =>
if (!ready_ && !start_ && manager_ != 0) {
// Current manager is server =>

if (manager_->getId() == manager_->getServerId()) {	
int nbr = 0;
for (auto it = humans_.begin(); it != humans_.end(); ++it) {
std::swap(players_[it->second],players_[players_.size()-nbr-1]);
++nbr;
}
humans_.clear();

for (int i = 0; i < nbr; ++i) {
delete players_.back();
players_.pop_back();
}


// Add new humans.
for (auto it = humans.begin(); it != humans.end(); ++it) {
humans_.push_back(PairHumanIndex(*it,-1));
}

for (auto it = humans_.begin(); it != humans_.end(); ++it) {
PairHumanIndex& pair = *it;
pair.second = players_.size();
players_.push_back(new Player());
}

// To update the changes to all clients.
sendServerInfo();
} else { // Current manager is client =>
humans_.clear(); // Clear old data.
for (auto it = humans.begin(); it != humans.end(); ++it) {
humans_.push_back(PairHumanIndex(*it,-1));
}
sendClientInfo();
}
return true;
}
return false;
}*/

const std::vector<Player*>& PlayerManager::getPlayers() const {
	return players_;
}
// private

