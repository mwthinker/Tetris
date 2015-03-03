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

TetrisGame::TetrisGame() : 
	pause_(false), start_(false),
	status_(WAITING_TO_CONNECT), nbrOfPlayers_(0),
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
		
		// Add human players.
		for (const DevicePtr& device : devices) {
			localUser_.addPlayer(width_, height_, device);
		}
		// Add computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			localUser_.addPlayer(width_, height_, std::make_shared<Computer>(ais_[i]));
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
		
		// Add human players.
		for (const DevicePtr& device : devices) {
			localUser_.addPlayer(width_, height_, device);
		}
		// Add computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			localUser_.addPlayer(width_, height_, std::make_shared<Computer>(ais_[i]));
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
		
		// Add human players.
		for (const DevicePtr& device : devices) {
			localUser_.addPlayer(width_, height_, device);
		}
		// Add computer players.
		for (int i = 0; i < nbrOfComputers; ++i) {
			localUser_.addPlayer(width_, height_, std::make_shared<Computer>(ais_[i]));
		}
		network_.startClient(ip, port);
		//clientSendClientInfo();
	}
}

void TetrisGame::startGame() {
	// Is a network game?
	if (network_.isServer() || network_.isClient()) {
		// Check if all players are ready to start!
		if (!localUser_.isReady() && !areRemoteConnectionsReady()) {
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
		
	// Init all players.
	std::vector<std::shared_ptr<Player>> players;
	for (auto& player : localUser_) {
		players.push_back(player);
	}
	for (auto& remoteConnection : remoteConnections_) {
		for (auto& player : *remoteConnection) {
			players.push_back(player);
		}
	}

	localUser_.restart();
	
	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	gameHandler_->initGame(players);
		
	// Signals the gui that the game is not paused.
	eventHandler_(GamePause(pause_));
}

void TetrisGame::restartGame() {
	startGame();
}

// Stops the game and aborts any active connection.
void TetrisGame::closeGame() {
	start_ = false;
	localUser_.setReady(false);
	status_ = WAITING_TO_CONNECT;

	// Disconnecting
	network_.stop();

	remoteConnections_.clear();
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
		localUser_.setReady(true);
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
	for (auto& connection : remoteConnections_) {
		connection->send(packet);
	}
}

void TetrisGame::receiveNetworkData() {
	if (network_.isServer()) {
		auto connection = network_.pollConnection();

		if (connection) {
			// Add the new connection.
			//remoteConnections_.push_back(std::make_shared<RemoteConnection>(++playerId_));
		}

		// Remove the disconnected connections.
		for (auto& client : remoteConnections_) {
			if (client->isActive()) {
				std::remove_if(remoteConnections_.begin(), remoteConnections_.end(), 
					[client](const std::shared_ptr<RemoteConnection>& remote) {
					
					return remote == client;
				});
			}
		}

		for (auto& client : remoteConnections_) {
			net::Packet packet;
			while (client->pollReceivePacket(packet)) {
				net::Packet packet;
				serverReceive(client, packet);
			}
		}
	} else if (network_.isClient()) {
		net::Packet packet;
		while (localUser_.pollReceivePacket(packet)) {
			net::Packet packet;
			clientReceive(packet);
		}
	}
}

void TetrisGame::update(Uint32 deltaTime) {
	receiveNetworkData();

	if (start_) {
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
				packet.reset();
				client->receive(packet);

				// Send the info of the new remote client to all remote clients.
				sendServerInfo();
			} else {
				throw mw::Exception("Protocol error, game already started!\n");
			}
			break;
		case PacketType::STARTGAME:
			startGame();
		case PacketType::MOVE:
			// Fall through!
		case PacketType::READY:
			// Fall through!
		case PacketType::TETRIS:
			// Fall through!
		case PacketType::STARTBLOCK:
			// Fall through!
			packet.reset();
			client->receive(packet);
			break;
	}
}

void TetrisGame::clientReceive(net::Packet& packet) {
	PacketType type;
	packet >> type;
	int id;
	packet >> id;
	auto it = std::find_if(remoteConnections_.begin(), remoteConnections_.end(), [id](const std::shared_ptr<RemoteConnection>& remote) {
		return remote->getId() == id;
	});

	if (it != remoteConnections_.end()) {

	}

	auto remote = *it;

	switch (type) {
		case PacketType::CLIENTINFO:
			// Game not started?
			if (!start_) {
				packet.reset();
				remote->receive(packet);

				// Send the info of the new remote client to all remote clients.
				sendServerInfo();
			} else {
				throw mw::Exception("Protocol error, game already started!\n");
			}
			break;
		case PacketType::STARTGAME:
			startGame();
		case PacketType::PAUSE:
			pause();
			break;
		case PacketType::MOVE:
			// Fall through!
		case PacketType::READY:
			// Fall through!
		case PacketType::TETRIS:
			// Fall through!
		case PacketType::STARTBLOCK:
			// Fall through!
			packet.reset();
			remote->receive(packet);
			break;
	}
}

bool TetrisGame::areRemoteConnectionsReady() {
	for (auto& remote : remoteConnections_) {
		if (!remote->isReady()) {
			return false;
		}
	}
	return true;
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
	remoteConnections_.clear();
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
	for (auto& remoteConnection : remoteConnections_) {
		for (auto& player : *remoteConnection) {
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
