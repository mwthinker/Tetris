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
	countDown_(3000), localUser_(sender_) {

}

TetrisGame::~TetrisGame() {
	closeGame();
}

void TetrisGame::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int width, int height, int maxLevel) {
	if (status_ == WAITING_TO_CONNECT) {
		localUser_.clear();

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
		lastConnectionId_ = 1;
		localUser_.setId(lastConnectionId_);
		localUser_.clear();

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
	if (!start_) {
		// Is a network game?
		if (network_.isServer()) {
			// Stops new connections.
			network_.setAcceptConnections(false);

			// Go from looby to game.
			net::Packet packet;
			packet << PacketType::START;
			sender_.sendToAll(packet);
		}
	} else { // Restart the network game.
		if (network_.isServer() || network_.isClient()) {
			// Go from looby to game.
			net::Packet packet;
			packet << PacketType::CONNECTION_RESTART;
			sender_.sendToAll(packet);
		}
	}

	initGame();
}

void TetrisGame::initGame() {
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
	for (auto& remoteConnection : sender_.remoteConnections_) {
		for (auto& player : *remoteConnection) {
			players.push_back(player);
		}
	}

	localUser_.restart();

	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	gameHandler_->initGame(players);

	// Signal the gui that the game is not paused.
	eventHandler_(GamePause(pause_));
}

void TetrisGame::restartGame() {
	startGame();
}

// Stop the game and abort any active connection.
void TetrisGame::closeGame() {
	start_ = false;
	status_ = WAITING_TO_CONNECT;

	// Disconnecting.
	network_.stop();

	sender_.remoteConnections_.clear();
	localUser_.clear();
}

bool TetrisGame::isPaused() const {
	return pause_;
}

void TetrisGame::pause() {
	if (start_) {
		pause_ = !pause_;
		if (sender_.active_) {
			net::Packet packet;
			packet << PacketType::PAUSE;
			sender_.sendToAll(packet);
		}
	}
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

void TetrisGame::receiveNetworkData() {
	if (network_.isServer()) {
		auto connection = network_.pollConnection();

		if (connection) {
			// Add the new connection.
			sender_.addRemoteConnection(lastConnectionId_);
		}

		// Remove the disconnected connections.
		sender_.removeDisconnectedRemoteConnections();

		for (auto& client : sender_.remoteConnections_) {
			net::Packet packet;
			while (client->pollReceivePacket(packet)) {
				net::Packet packet;
				serverReceive(client, packet);
			}
		}
	} else if (network_.isClient()) {
		net::Packet packet;
		while (sender_.clientConnection_->pollReceivePacket(packet)) {
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
		case PacketType::START:
			//startGame();
			break;
		case PacketType::CONNECTION_INFO:
			packet.reset();
			client->receive(packet);
			// Send the server's version of the packet in order for the  
			// client to have the correct id.
			sender_.sendToAllExcept(client, client->getClientInfo());
			break;
			// Fall through!
		case PacketType::PLAYER_MOVE:
			// Fall through!
		case PacketType::PLAYER_TETRIS:
			// Fall through!
		case PacketType::PLAYER_START_BLOCK:
			// Fall through!
		case PacketType::PLAYER_NAME:
			// Fall through!
		case PacketType::PLAYER_LEVEL:
			// Fall through!
		case PacketType::PLAYER_POINTS:
			packet.reset();
			sender_.sendToAllExcept(client, packet);
			client->receive(packet);
			break;
	}
}

void TetrisGame::clientReceive(net::Packet& packet) {
	PacketType type;
	packet >> type;
	int id;
	packet >> id;

	switch (type) {
		case PacketType::CONNECTION_INFO:
		{
			// Game not started?
			auto remote = sender_.findRemoteConnection(id);
			if (remote == nullptr) {
				// Add new connection.
				remote = sender_.addRemoteConnection(id);
			}
			packet.reset();
			remote->receive(packet);
			break;
		}
		case PacketType::CONNECTION_ID:
			localUser_.setId(id);
			break;
		case PacketType::CONNECTION_RESTART:
			startGame();
			break;
		case PacketType::PAUSE:
			pause_ = !pause_;
			break;
		case PacketType::PLAYER_LEVEL:
			// Fall through!
		case PacketType::PLAYER_POINTS:
			// Fall through!
		case PacketType::PLAYER_MOVE:
			// Fall through!
		case PacketType::PLAYER_TETRIS:
			// Fall through!
		case PacketType::PLAYER_START_BLOCK:
		{
			auto remote = sender_.findRemoteConnection(id);
			if (remote != nullptr) {
				packet.reset();
				remote->receive(packet);
			} else {
				// Protocol error.
			}
			break;
		}
	}
}

void TetrisGame::sendServerInfo(net::Connection connection) {
	NewConnection newConnection(NewConnection::SERVER);
	nbrOfPlayers_ = 0;
	// Add new player to all human players.
	net::Packet packet;
	packet << PacketType::CONNECTION_BOARD_SIZE;
	packet << width_ << height_;
	sender_.sendToAll(packet);
	packet.reset();

	packet << PacketType::CONNECTION_INFO;
	for (auto& player : localUser_) {
		packet << player->getId();
	}
	
	for (auto& remote : sender_.remoteConnections_) {
		connection.send(remote->getClientInfo());
	}

	eventHandler_(newConnection);
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
					
					//if (status_ == LOCAL && player->) {
					//	eventHandler_(GameOver(pInfo.points_));
					//}
					//
				}
			}
			break;
	}
	

	try {
		auto localPlayer = dynamic_cast<LocalPlayer&>(player);
		// Rows removed from local player?
		if (rows != 0) {
			// Assign points and number of cleared rows.
			localPlayer.setClearedRows(localPlayer.getClearedRows() + rows);
			localPlayer.setPoints(localPlayer.getPoints() + player.getLevel() * rows * rows);

			// Multiplayer?
			if (nbrOfPlayers_ > 1) {
				// Increase level up counter for all opponents to the current player.
				// Remote players will be added indirectly.
				for (auto& opponent : localUser_) {
					if (opponent->getId() != localPlayer.getId()) {
						opponent->setLevelUpCounter(opponent->getLevelUpCounter() + rows);
					}
				}
			} else { // Singleplayer!
				localPlayer.setLevelUpCounter(localPlayer.getLevelUpCounter() + rows);
			}

			// Set level.
			int level = (localPlayer.getLevelUpCounter() / ROWS_TO_LEVEL_UP) + 1;
			if (level <= maxLevel_) {
				localPlayer.setLevel(level);
			}
		}
	} catch (std::exception& e) {
		// Reference cast failed, is therefore a remote player. Do nothing!
	}
}

void TetrisGame::Sender::sendToAll(const net::Packet& packet) const {
	sendToAllExcept(nullptr, packet);
}

void TetrisGame::Sender::sendToAllExcept(std::shared_ptr<RemoteConnection> remoteSendNot, const net::Packet& packet) const {
	if (clientConnection_) {
		clientConnection_->send(packet);
	} else {
		for (auto& connection : remoteConnections_) {
			if (remoteSendNot != connection) {
				connection->send(packet);
			}
		}
	}
}

std::shared_ptr<RemoteConnection> TetrisGame::Sender::findRemoteConnection(int connectionId) {
	auto it = std::find_if(remoteConnections_.begin(), remoteConnections_.end(),
		[connectionId](const std::shared_ptr<RemoteConnection>& remote) {

		return remote->getId() == connectionId;
	});

	if (it == remoteConnections_.end()) {
		return nullptr;
	}
	return *it;
}

std::shared_ptr<RemoteConnection> TetrisGame::Sender::addRemoteConnection(int connectionId) {
	// Add the new connection.
	auto remote = std::make_shared<RemoteConnection>(++connectionId);
	remoteConnections_.push_back(remote);
	return remote;
}

void TetrisGame::Sender::removeDisconnectedRemoteConnections() {
	for (auto& client : remoteConnections_) {
		if (client->isActive()) {
			std::remove_if(remoteConnections_.begin(), remoteConnections_.end(),
				[client](const std::shared_ptr<RemoteConnection>& remote) {

				return remote == client;
			});
		}
	}
}
