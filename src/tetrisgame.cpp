#include "tetrisgame.h"

#include "tetrisboard.h"
#include "player.h"
#include "remoteplayer.h"
#include "localplayer.h"
#include "tetrisgameevent.h"
#include "localplayer.h"
#include "computer.h"
#include "device.h"
#include "tetrisparameters.h"
#include "protocol.h"

#include <mw/exception.h>
#include <net/packet.h>

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>

namespace {

	net::Packet createReady(bool ready) {
		net::Packet packet;
		packet << PacketType::READY;
		return packet;
	}

	// char type = MOVE
	// char playerId
	// char move
	// char nextBlockType, before move has taken effect
	net::Packet createMove(int playerId, Move move, BlockType next) {
		net::Packet packet;
		packet << PacketType::MOVE;
		packet << playerId;
		packet << move;
		packet << next;
		return packet;
	}

	net::Packet createPause() {
		net::Packet packet;
		packet << PacketType::PAUSE;
		return packet;
	}

	net::Packet createTetrisInfo(int playerId, const std::vector<BlockType>& blockTypes) {
		net::Packet packet;
		packet << PacketType::TETRIS;
		packet << playerId;
		for (BlockType type : blockTypes) {
			packet << type;
		}
		return packet;
	}
	/*
	net::Packet createStartBlock(const UserConnection& userConnection) {
		net::Packet packet;
		packet << PacketType::STARTBLOCK;
		for (PlayerPtr player : userConnection) {
			packet << player->getCurrentBlock();
			packet << player->getNextBlock();
		}
		return packet;
	}
	*/

}

TetrisGame::TetrisGame() : 
	pause_(false), start_(false),
	status_(WAITING_TO_CONNECT), nbrOfPlayers_(0), playerId_(0),
	countDown_(3000) {

}

TetrisGame::~TetrisGame() {
	closeGame();
}

void TetrisGame::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int width, int height, int maxLevel) {
	if (status_ == WAITING_TO_CONNECT) {
		localUser_.clear();
		localUser_.setReady(true);

		width_ = width;
		height_ = height;
		maxLevel_ = maxLevel;
		status_ = LOCAL;
		playerId_ = 0;
		for (const DevicePtr& device : devices) {
			localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, device));
		}

		// Add computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, std::make_shared<Computer>(ais_[i])));
		}
		nbrOfPlayers_ = localUser_.getSize();
	}	
}

void TetrisGame::createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int port, int width, int height, int maxLevel) {
	if (status_ == WAITING_TO_CONNECT) {
		localUser_.clear();
		localUser_.setReady(true);

		width_ = width;
		height_ = height;
		maxLevel_ = maxLevel;
		status_ = SERVER;
		playerId_ = 0;
		// Add local human players.
		for (const DevicePtr& device : devices) {
			localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, device));
		}
		// Add local computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, std::make_shared<Computer>(ais_[i])));
		}
		network_.startServer(port);
		network_.setAcceptConnections(true);
	}
}

void TetrisGame::createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int port, std::string ip, int maxLevel) {
	
	if (status_ == WAITING_TO_CONNECT) {
		localUser_.clear();
		localUser_.setReady(true);
		
		maxLevel_ = maxLevel;
		status_ = SERVER;
		playerId_ = 0;
		// Add local human players.
		for (const DevicePtr& device : devices) {
			//localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, device));
		}
		// Add local computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			//localUser_.add(std::make_shared<LocalPlayer>(++playerId_, width_, height_, std::make_shared<Computer>(ais_[i])));
		}
		network_.startClient(ip, port);
		clientSendClientInfo();
	}
}

void TetrisGame::startGame() {
	// Is a server?
	if (network_.isServer()) {
		// Check if all players are ready to start!
		if (!isAllUsersReady()) {
			// Not ready -> don't start!
			return;
		}

		// Stops new connections.
		network_.setAcceptConnections(false);

		// Only the server can start/restart the game!
		net::Packet packet;
		packet << PacketType::STARTGAME;
		serverSendToAll(packet);
	}

	if (!start_) {
		countDown_ = 3000;
		
		// Signals the gui that the game begins.
		switch (status_) {
			case TetrisGame::LOCAL:
				eventHandler_(GameStart(GameStart::LOCAL));
				break;
			case TetrisGame::CLIENT:
				eventHandler_(GameStart(GameStart::CLIENT));
				break;
			case TetrisGame::SERVER:
				eventHandler_(GameStart(GameStart::SERVER));
				break;
		}

		start_ = true;
		pause_ = false;

		// When server decides to start, all clients must be ready.
		// If not they are set to ready. To avoid sync problem.
		/*
		iterateUserConnections([&](UserConnection& user) {
			user.setReady(true);
			return true;
		});

		//sendStartBlock();
		std::vector<PlayerPtr> players;
		iterateAllPlayers([&players](PlayerPtr player) {
			players.push_back(player);
			player->setPlayerInfo(PlayerInfo());
			return true;
		});
		
		nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
		gameHandler_->initGame(players);
		*/
		// Signals the gui that the game is not paused.
		eventHandler_(GamePause(pause_));
	}
}

void TetrisGame::restartGame() {
	// Game is started?
	if (start_) {
		// Is a server?
		if (network_.isServer()) {
			// Only the server can restart the game!
			net::Packet packet;
			packet << PacketType::STARTGAME;
			serverSendToAll(packet);
		}

		// Is a server or a local game?
		if (!network_.isClient()) {
			startGame();
		}
	}
}

// Stops the game and aborts any active connection.
void TetrisGame::closeGame() {
	start_ = false;
	localUser_.setReady(false);
	status_ = WAITING_TO_CONNECT;

	// Disconnecting
	network_.stop();

	users_.clear();
	localUser_.clear();
}

bool TetrisGame::isPaused() const {
	return pause_;
}

void TetrisGame::pause() {
	if (isStarted()) {
		//sendPause();
	}
}

void TetrisGame::changeReadyState() {
	if (!isStarted()) {
		//sendReady();
	}
}

bool TetrisGame::isReady() const {
	return localUser_.isReady();
}

bool TetrisGame::isStarted() const {
	return start_;
}

void TetrisGame::addCallback(const mw::Signal<TetrisGameEvent&>::Callback& callback) {
	eventHandler_.connect(callback);
}

int TetrisGame::getNbrOfPlayers() const {
	return nbrOfPlayers_;
}

void TetrisGame::serverSendToAll(const net::Packet& packet) {
	//for (auto& connection : serverConnections_) {
	//	connection->send(packet);
	//}
}

void TetrisGame::receiveNetworkData() {
	if (network_.isServer()) {
		auto connection = network_.pollConnection();

		if (connection) {
			// Add the new connection.
			//users_.emplace_back(++playerId_);
		}

		// Remove the disconnected connections.
		for (auto& client : users_) {
			if (client->isActive()) {
				//std::remove_if(users_.begin(), users_.end(), [](const UserConnectionPtr& user) {
				//	return tmp->getId() == clientId;
				//});
				//serverConnections_.erase(connection);
			}
		}

		for (auto& client : users_) {
			net::Packet packet;
			while (client->receive(packet)) {
				net::Packet packet;
				serverReceive(client, packet);
			}
		}
	} else if (network_.isClient()) {
		net::Packet packet;
		//while (clientConnection_->receive(packet)) {
		//	net::Packet packet;
		//	clientReceive(packet);
		//}
	}
}

void TetrisGame::update(Uint32 deltaTime) {
	receiveNetworkData();

	if (isStarted()) {
		if (!pause_) {
			countDown_ -= deltaTime;
			if (countDown_ < 0) {
				if (countDown_ > -20000) { // -20000 = Time impossible to reach.
					gameHandler_->countDown(countDown_);
				}
				localUser_.updateGame(deltaTime);
				countDown_ = -20000;
			} else {
				gameHandler_->countDown(countDown_);
			}

			// Update the board for the local users and send updates to remote players.
			//for (PlayerPtr& player : localUser_) {
				//Move move;
				//BlockType next;
				// Update the local player tetris board and send the input to all remote players.
				//while (player->updateBoard(move, next)) {
					//sendMove(player->getId(), move, next);
				//}
			//}
		}
	}
}

void TetrisGame::serverReceive(std::shared_ptr<RemoteConnection> client, net::Packet& packet) {
	PacketType type;
	packet >> type;
	switch (type) {
		case PacketType::CLIENTINFO:
			// Game not started?
			if (!start_) {
				int nbrOfPlayers;
				packet >> nbrOfPlayers;

				// Number of players to add or remove.
				client->clear();

				for (int i = 0; i < nbrOfPlayers; ++i) {
					auto player = std::make_shared<RemotePlayer>(++playerId_, width_, height_);
					client->add(player);
					char length;
					packet >> length;
					std::string name;
					packet >> name;
					//player->setName(name);
				}

				// Send the info of the new remote client to all remote clients.
				sendServerInfo();
			} else {
				throw mw::Exception("Protocol error, game already started!\n");
			}
			break;
		case PacketType::STARTGAME:
			startGame();
			break;
	}
}

void TetrisGame::clientReceive(net::Packet& packet) {

}

// Receives data from a client or server.
// First element must be a value defined in PacketType.
void TetrisGame::receiveData(net::Packet& packet, int id) {
	PacketType type;
	packet >> type;
	switch (type) {
		case PacketType::MOVE:
		{
			// Game not started?
			if (!start_) {
				throw mw::Exception("Protocol error!\n");
			}

			int playerId;
			Move move;
			BlockType next;
			receivInput(packet, playerId, move, next);
			/*
			// Remote player?
			if (network_->getId() != id) {
				// Iterate until the player is found.
				iterateAllPlayers([&](PlayerPtr player) {
					bool findPlayer = false;
					if (player->getId() == playerId) {
						player->update(move, next);
						findPlayer = true;
					}
					return !findPlayer;
				});
			}
			*/
			break;
		}
		case PacketType::READY:
			// Game started?
			if (start_) {
				// Don't change anything, break!
				break;
			}

			// Finds user and change the ready state.
			/*iterateUserConnections([&](UserConnection& user) {
				if (user.getId() == id) {
					user.setReady(!user.isReady());
					eventHandler_(GameReady(user.getId(), user.isReady()));
					return false; // User found, stop looking!
				}
				// User not found, keep iterating!!
				return true;
			});*/
			break;
		case PacketType::TETRIS:
		{
			// Game not started?
			if (!start_) {
				throw mw::Exception("Protocol error!\n");
			}
			/*
			if (data.size() < 2) {
				throw mw::Exception("Protocol error!\n");
			}
			
			int playerId = data[1];
			int index = 2;
			std::vector<BlockType> blockTypes;
			while (index < data.size()) {
				blockTypes.push_back(static_cast<BlockType>(data[index]));
				++index;
			}
			*/
			// Add rows to the player with correct id.
			/*iterateAllPlayers([&](PlayerPtr player) {
				bool findPlayer = false;
				
				if (player->getId() == playerId) {
					player->update(blockTypes);
					findPlayer = true;
				}
				
				return !findPlayer;
			});*/
			break;
		}
		case PacketType::STARTGAME:
			// Signals the game to start. Only sent by the server.
			/*
			// Is not sended by server?
			if (mw::Network::SERVER_ID != id) {
				throw mw::Exception("Protocol error!\n");
			}
			*/
			clientStartGame();

			countDown_ = 3000;
			break;
		case PacketType::PAUSE:
			/*
			if (data.size() != 1) {
				throw mw::Exception("Protocol error!\n");
			}
			pause_ = !pause_;
			*/
			// Signals the gui that the game begins.
			eventHandler_(GamePause(pause_));

			countDown_ = 3000;
			break;
		case PacketType::STARTBLOCK:
			/*
			// Remote user?
			if (network_->getId() != id) {
				receiveStartBlock(data, id);
			}
			*/
			break;
		case PacketType::CLIENTINFO:
			// Sent by yourself to the server. Do nothing.
			break;
		default:
			throw mw::Exception("Protocol error!\n");
			break;
	}
}

bool TetrisGame::isAllUsersReady() {
	bool allUsersReady = true;
	/*
	iterateUserConnections([&](UserConnection& user) {
		if (user.isReady()) {
			return true;
		}
		allUsersReady = false;
		return false;
	});*/

	return allUsersReady;
}

// char type = STARTINFO
// char id1
// char nbrOfPlayers1 = N1
// char player11Id
// ..............
// char player1NId
// char id2 = N2
// char nbrOfPlayers2 = N2
// char player2Id
// ..............
// char player2NId
void TetrisGame::sendServerInfo() {
	NewConnection newConnection(NewConnection::SERVER);
	nbrOfPlayers_ = 0;
	// Add new player to all human players.
	net::Packet packet;
	packet << PacketType::SERVERINFO;
	packet << width_ << height_;
	/*
	iterateUserConnections([&](const UserConnection& user) {
		packet << user.getId();
		packet << user.isReady();
		packet << user.getNbrOfPlayers();
		nbrOfPlayers_ += user.getNbrOfPlayers();
		newConnection.add(user.getId(), user.getNbrOfPlayers(), user.isReady());
		for (PlayerPtr player : user) {
			packet << player->getId();
			std::string name = player->getName();
			packet << (int) name.length();
			for (char chr : name) {
				packet << chr;
			}
		}
		return true;
	});
	*/
	eventHandler_(newConnection);
	serverSendToAll(packet);
}

void TetrisGame::clientReceiveServerInfo(net::Packet packet) {
	users_.clear();
	nbrOfPlayers_ = 0;
	NewConnection newConnection(NewConnection::CLIENT);
	packet >> width_;
	packet >> height_;

	int index = 2;
	while (packet.dataLeftToRead()) {
		int id;
		packet >> id;
		bool ready;
		packet >> ready;
		int nbrOfPlayers;
		packet >> nbrOfPlayers;
		nbrOfPlayers_ += nbrOfPlayers;

		newConnection.add(id, nbrOfPlayers, ready);
		/*
		auto user = std::make_shared<LocalConnection<LocalPlayer>>(id);
		// The id for the local user?
		if (id == localUser_.getId()) {
			//localUser_ = user;
		}
		//users_.push_back(user);
		for (int i = 0; i < nbrOfPlayers; ++i) {
			int playerId;
			packet >> playerId;

			PlayerPtr player;
			// The id for the local user?
			if (id == localUser_.getId()) {
				player = std::make_shared<LocalPlayer>(playerId, width_, height_, devices_[i]);
			} else { // Remote user.
				player = std::make_shared<RemotePlayer>(playerId, width_, height_);
			}
			//user->add(player);
			user->setReady(ready);
			std::string name;
			packet >> name;
			player->setName(name);
		}
		*/
	}
	eventHandler_(newConnection);
}

// char type = CLIENTINFO
// char nbrOfPlayers
void TetrisGame::clientSendClientInfo() {
	net::Packet packet;
	packet << PacketType::CLIENTINFO;
	/*
	packet << (int) devices_.size();
	for (auto& device : devices_) {
		packet << device->getPlayerName();
	}
	*/
	//clientConnection_->send(packet);
}

void TetrisGame::receivInput(net::Packet& packet, int& playerId, Move& move, BlockType& next) {
	PacketType type;
	packet >> type;
	packet >> playerId;
	packet >> move;
	packet >> next;
}

void TetrisGame::receiveStartBlock(net::Packet& packet, int id) {
	//UserConnectionPtr user = nullptr;
	
	/*
	auto it = std::find_if(users_.begin(), users_.end(), [id](const UserConnectionPtr& user) {
		return user->getId() == id;
	});
	

	if (it == users_.end()) {
		throw mw::Exception("Protocol error!\n");
	}

	for (auto& player : *user) {
		BlockType current;
		packet >> current;
		BlockType next;
		packet >> next;
		player->restart();
		player->update(current, next);
	}
	*/
}

void TetrisGame::clientStartGame() {
	// Game already started?
	if (start_) {
		// Restart local players.
		
		for (auto& player : localUser_) {
			// Restart player.
			//player->restart();
		}
	} else {
		// Signals the gui that the game begins.
		switch (status_) {
			case TetrisGame::LOCAL:
				eventHandler_(GameStart(GameStart::LOCAL));
				break;
			case TetrisGame::CLIENT:
				eventHandler_(GameStart(GameStart::CLIENT));
				break;
			case TetrisGame::SERVER:
				eventHandler_(GameStart(GameStart::SERVER));
				break;
		}
	}

	start_ = true;
	pause_ = false;
	// When server decides to start all clients must be ready.
	// If not they are set to ready. To avoid sync problem.
	std::vector<std::shared_ptr<Player>> players;
	players.insert(players.end(), localUser_.begin(), localUser_.end());
	for (auto& user : users_) {
		user->setReady(true);
		for (auto& player : *user) {
			players.push_back(player);
		}
	}

	//sendStartBlock();
	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	gameHandler_->initGame(players);
	
	// Signals the gui that the game is not paused.
	eventHandler_(GamePause(pause_));
}

void TetrisGame::applyRules(Player& player, GameEvent gameEvent) {
	//PlayerInfo& pInfo = player.getPlayerInfo();
	int rows = 0;
	switch (gameEvent) {
		case GameEvent::ONE_ROW_REMOVED:
			rows = 1;
			break;
		case GameEvent::TWO_ROW_REMOVED:
			rows = 2;
			break;
		case GameEvent::THREE_ROW_REMOVED:
			rows = 3;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			rows = 4;
			// Multiplayer?
			if (nbrOfPlayers_ > 1) {
				// Add rows only for local players. Remote players will add
				// indirectly.
				for (auto& local : localUser_) {
					if (player.getId() != local->getId()) {
						std::vector<BlockType> blockTypes;
						for (int i = 0; i < rows; ++i) {
							std::vector<BlockType> tmp = generateRow(local->getTetrisBoard(), 0.79);
							blockTypes.insert(blockTypes.begin(), tmp.begin(), tmp.end());
						}
						//sendTetrisInfo(local->getId(), blockTypes);
					}
				}
			}
			break;
		case GameEvent::GAME_OVER:
			// Multiplayer?
			if (nbrOfPlayers_ > 1) {
				// One player more is dead.
				--nbrOfAlivePlayers_;

				// All dead except one => End game!
				if (nbrOfAlivePlayers_ == 1) {
					/*
					iterateAllPlayers([&](PlayerPtr player) {
						// Will be noticed in the next call to applyRules(...).
						// Nothing happens if player allready dead.
						player->update(Move::GAME_OVER);
						return true;
					});
					*/
				}
			} else { // Singleplayer.
				// And is the correct settings?				
				if (player.getTetrisBoard().getRows() == TETRIS_HEIGHT
					&& player.getTetrisBoard().getColumns() == TETRIS_WIDTH
					&& maxLevel_ == TETRIS_MAX_LEVEL) {

					// Is local and a human player?
					//
					//if (status_ == LOCAL && !player.isAi()) {
					//	eventHandler_(GameOver(pInfo.points_));
					//}
					//
				}
			}
			break;
	}

	if (rows != 0) {
		// Assign points and number of cleared rows.
		//pInfo.nbrClearedRows_ += rows;
		//pInfo.points_ += player.getLevel() * rows * rows;

		// Multiplayer?
		if (nbrOfPlayers_ > 1) {
			// Increase level up counter for all opponents to the current player.
			/*
			iterateAllPlayers([&](PlayerPtr opponent) {
				if (opponent->getId() != player.getId()) {
					PlayerInfo& tmpInfo = opponent->getPlayerInfo();
					tmpInfo.levelUpCounter_ += rows;
				}
				return true;
			});
			*/
		} else { // Singleplayer!
			//pInfo.levelUpCounter_ += rows;
		}

		// Set level.
		//int level = (pInfo.levelUpCounter_ / ROWS_TO_LEVEL_UP) + 1;
		//if (level <= maxLevel_) {
		//	player.setLevel(level);
		//}
	}
}
