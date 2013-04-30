#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"
#include "player.h"
#include "human.h"
#include "inputdevice.h"
#include "inputkeyboard.h"
#include "inputjoystick.h"

#include <mw/sound.h>

#include <mw/servermanager.h>
#include <mw/clientmanager.h>
#include <mw/localmanager.h>

#include <iostream>
#include <utility>
#include <memory>
#include <sstream>

TetrisGame::TetrisGame() {
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

TetrisGame::~TetrisGame() {
	closeGame();
	delete manager_;
}

void TetrisGame::createNewHumanPlayers(int nbrOfLocalPlayers) {
	if (status_ == WAITING_TO_CONNECT) {
		humanPlayers_.clear();	
		for (int i = 0; i < nbrOfLocalPlayers; ++i) {
			HumanPtr human(new Human());
			humanPlayers_.push_back(human);
		}
	}
}

void TetrisGame::createLocalGame(int nbrOfLocalPlayers) {
	createNewHumanPlayers(nbrOfLocalPlayers);
	connect(humanPlayers_,LOCAL);
}

void TetrisGame::createServerGame(int nbrOfLocalPlayers, int port) {
	createNewHumanPlayers(nbrOfLocalPlayers);
	setServerPort(port);
	connect(humanPlayers_,SERVER);
}

void TetrisGame::createClientGame(int nbrOfLocalPlayers,int port, std::string ip) {	
	if (status_ == WAITING_TO_CONNECT) {
		createNewHumanPlayers(nbrOfLocalPlayers);
		setConnectToIp(ip);
		setConnectToPort(port);
		connect(humanPlayers_,CLIENT);
	}
}

void TetrisGame::startGame() {
	// Game not started. // Connection must be active!
	if (manager_ != 0 && !start_ && manager_->getStatus() == mw::ConnectionManager::ACTIVE) {		
		// Is server.
		if (manager_->getId() == manager_->getServerId()) {
			if (!ready_) {
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

void TetrisGame::restartGame() {
	// Manager is active and is server and game is started => restart,
	if (manager_ != 0 && isStarted() && manager_->getId() == manager_->getServerId()) {
		serverSendStartGame();
	}
}

// Stops the game and aborts any active connection.
void TetrisGame::closeGame() {	
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

void TetrisGame::setReadyGame(bool ready) {
	if (manager_ != 0 && !isStarted()) {
		ready_ = ready;
		sendReady();
	}
}

bool TetrisGame::isReady() const {
	return ready_;
}

void TetrisGame::setInputDevice(const InputDevicePtr& inputDevice, int playerIndex) {
	inputDevices_[playerIndex] = inputDevice;
}

void TetrisGame::update(Uint32 deltaTime) {
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

	// Takes care of user input for all human players.
	for (auto it = humanPlayers_.begin(); it != humanPlayers_.end(); ++it) {
		HumanPtr& human = *it;
		unsigned int index = it - humanPlayers_.begin();
		if (index < inputDevices_.size()) {
			InputDevicePtr& device = inputDevices_[index];

			PlayerEvent playerEvent;
			while (device->pollEvent(playerEvent)) {
				human->updatePlayerEvent(playerEvent);
			}
		}
	}

	if (start_) {
		auto& players = players_;
		// Iterate through all players and progress the game and apply game rules and sound effects.
		for (Player* player : players) {
			GameEvent gameEvent;
			while (player->pollGameEvent(gameEvent)) {
				applyRules(player, gameEvent);
				soundEffects(gameEvent);
			}
		}
	}
}

void TetrisGame::draw() {
	glPushMatrix();	
	
	auto& players = players_;
	for (auto it = players.begin(); it != players.end(); ++it) {
		Player* player = *it;
		player->draw();
		glTranslated(player->getWidth(),0,0);
	}
	
	glPopMatrix();
}

double TetrisGame::getWidth() const {
	double width = 0;
	const auto& players = players_;
	
	for (Player* player : players) {
		width += player->getWidth();
	}
	return width;
}

double TetrisGame::getHeight() const {
	auto& players = players_;
	if (players.size() == 0) {
		return 400;
	}
	return players.front()->getHeight();
}

void TetrisGame::soundEffects(GameEvent gameEvent) {
	mw::Sound sound;
	switch (gameEvent) {
	case GRAVITY_MOVES_BLOCK:
		break;
	case BLOCK_COLLISION:
		sound = soundBlockCollision;
		break;
	case PLAYER_ROTATES_BLOCK:
		break;
	case ONE_ROW_REMOVED:
		// Fall through
	case TWO_ROW_REMOVED:
		// Fall through
	case THREE_ROW_REMOVED:
		sound = soundRowRemoved;
		break;
	case FOUR_ROW_REMOVED:
		sound = soundTetris;
		break;
	case GAME_OVER:
		break;
	default:
		break;
	}
	sound.play();
}

// Initiates the choosen connection.
void TetrisGame::connect(const std::vector<HumanPtr>& humans, Status status) {
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

void TetrisGame::applyRules(Player* player, GameEvent gameEvent) {
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

void TetrisGame::setConnectToPort(int port) {
	connectToPort_ = port;
}

int TetrisGame::getConnectToPort() const {
	return connectToPort_;
}

void TetrisGame::setServerPort(int port) {
	serverPort_ = port;
}

int TetrisGame::getServerPort() const {
	return serverPort_;
}

void TetrisGame::setConnectToIp(std::string ip) {
	connectToIp_ = ip;
}

std::string TetrisGame::getConnectToIp() const {
	return connectToIp_;
}

int TetrisGame::getNumberOfPlayers(int connection) const {
	if (connection == 0) {
		return humans_.size();
	}
	return remoteUsers_[connection-1]->getNbrOfPlayers();
}
