#include "tetrisgame.h"

#include "tetrisboard.h"
#include "tetrisgameevent.h"
#include "localplayer.h"
#include "remoteplayer.h"
#include "tetrisparameters.h"
#include "protocol.h"

#include <net/packet.h>

#include <vector>
#include <algorithm>

TetrisGame::TetrisGame() :
	pause_(false),
	status_(WAITING_TO_CONNECT), nbrOfPlayers_(0),
	localConnection_(sender_),
	width_(TETRIS_WIDTH), height_(TETRIS_HEIGHT), maxLevel_(TETRIS_MAX_LEVEL),
	lastConnectionId_(CLIENT_UNDEFINED_CONNECTION_ID) {

}

TetrisGame::~TetrisGame() {
	closeGame();
}

void TetrisGame::createLocalGame() {
	if (status_ == WAITING_TO_CONNECT) {
		status_ = LOCAL;
		
		nbrOfPlayers_ = localConnection_.getSize();
		localConnection_.restart();
		initGame();
	}
}

void TetrisGame::createServerGame(int port) {
	if (status_ == WAITING_TO_CONNECT) {
		status_ = SERVER;

		lastConnectionId_ = SERVER_CONNECTION_ID;
		localConnection_.setId(lastConnectionId_);
		
		network_.startServer(port);
		network_.setAcceptConnections(true);
		initGame();
	}
}

void TetrisGame::createClientGame(int port, std::string ip) {
	if (status_ == WAITING_TO_CONNECT) {
		status_ = CLIENT;
		lastConnectionId_ = CLIENT_UNDEFINED_CONNECTION_ID;
		localConnection_.setId(lastConnectionId_);

		network_.startClient(ip, port);
	}
}

void TetrisGame::initGame() {
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
	
	pause_ = false;

	// Init all players.
	std::vector<std::shared_ptr<Player>> players;
	for (auto& player : localConnection_) {
		players.push_back(player);
	}
	for (auto& remoteConnection : sender_.remoteConnections_) {
		for (auto& player : *remoteConnection) {
			players.push_back(player);
		}
	}

	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	gameHandler_->initGame(players);

	// Signal the gui that the game is not paused.
	eventHandler_(GamePause(pause_));
}

void TetrisGame::restartGame() {
	if (status_ != Status::WAITING_TO_CONNECT) {
		if (network_.isServer() || network_.isClient()) {
			// Go from looby to game.
			net::Packet packet;
			packet << localConnection_.getId();
			packet << PacketType::RESTART;
			sender_.sendToAll(packet);

			localConnection_.restartAndSend();

			if (pause_) {
				pause_ = false;
				eventHandler_(GamePause(pause_));
			}

		} else {
			localConnection_.restart();
		}

		initGame();
	}
}

// Stop the game and abort any active connection.
void TetrisGame::closeGame() {
	status_ = WAITING_TO_CONNECT;

	// Disconnecting.
	network_.stop();

	sender_.remoteConnections_.clear();
	//sender_.clientConnection_->clear();
	localConnection_.clear();
}

bool TetrisGame::isPaused() const {
	return pause_;
}

void TetrisGame::pause() {
	pause_ = !pause_;
	if (sender_.isActive()) {
		net::Packet packet;
		packet << PacketType::PAUSE;
		packet << localConnection_.getId();
		sender_.sendToAll(packet);
	}
	eventHandler_(GamePause(pause_));
}

void TetrisGame::addCallback(const mw::Signal<TetrisGameEvent&>::Callback& callback) {
	eventHandler_.connect(callback);
}

int TetrisGame::getNbrOfPlayers() const {
	return nbrOfPlayers_;
}

void TetrisGame::resizeBoard(int width, int height) {
	if (width > 6 && width <= 40 &&
		height > 6 && height <= 40 &&
		(width_ != width || height_ != height)) {
		
		width_ = width;
		height_ = height;

		if (network_.isServer() || network_.isClient()) {
			net::Packet packet;
			packet << PacketType::BOARD_SIZE;
			packet << localConnection_.getId();
			packet << width << height;
			sender_.sendToAll(packet);

			localConnection_.resizeAndSend(width, height);
		} else {
			localConnection_.resizeBoard(width, height);
		}
		initGame();
	}
}

void TetrisGame::setPlayers(const std::vector<DevicePtr>& devices) {
	localConnection_.clear();
	
	// Add human players.
	localConnection_.addPlayers(10, 24, devices);

	if (status_ != Status::WAITING_TO_CONNECT) {
		if (network_.isServer() || network_.isClient()) {
			sender_.sendToAll(localConnection_.getClientInfo());
		}

		localConnection_.restart();
		initGame();
	}
}

void TetrisGame::receiveAndSendNetworkData() {
	if (network_.isServer()) {
		auto connection = network_.pollConnection();

		if (connection) {
			// Add the new connection.
			auto newRemote = sender_.addRemoteConnection(++lastConnectionId_, connection);
			
			// Send game info to the new connection.
			net::Packet packet;
			packet << PacketType::CONNECTION_ID;
			packet << newRemote->getId();
			connection->send(packet);
			
			packet.clear();
			packet << PacketType::BOARD_SIZE;
			packet << localConnection_.getId();
			packet << width_ << height_;
			connection->send(packet);

			// Send connection info of all connections to the new connection.
			connection->send(localConnection_.getClientInfo());
			for (auto& remote : sender_.remoteConnections_) {
				if (newRemote != remote) {
					newRemote->send(remote->getClientInfo());
				}
			}

			// Send the new connection info to the old connections.
			sender_.sendToAllExcept(newRemote, newRemote->getClientInfo());

			eventHandler_(NewConnection());
		}

		// Remove the disconnected connections.
		sender_.removeDisconnectedRemoteConnections();

		for (auto& client : sender_.remoteConnections_) {
			net::Packet packet;
			while (client->pollReceivePacket(packet)) {
				serverReceive(client, packet);
			}
		}
	} else if (network_.isClient()) {
		if (sender_.clientConnection_ == nullptr) {
			sender_.clientConnection_ = network_.pollConnection();
			if (sender_.clientConnection_) {
				eventHandler_(NewConnection());
			}
		} else {
			net::Packet packet;
			while (sender_.clientConnection_->receive(packet)) {
				clientReceive(packet);
			}
		}
	}
}

void TetrisGame::update(Uint32 deltaTime) {
	if (status_ != Status::WAITING_TO_CONNECT) {
		receiveAndSendNetworkData();
	
		if (!pause_) {
			localConnection_.updateGame(deltaTime);
		}
	}
}

void TetrisGame::serverReceive(std::shared_ptr<RemoteConnection> remoteConnection, net::Packet& packet) {
	packet.reset();
	PacketType type;
	packet >> type;
	int id;
	packet >> id;
	
	switch (type) {
		case PacketType::CONNECTION_INFO:
		{
			packet.reset();
			remoteConnection->receive(packet);
			// Send the server's version of the packet in order for the  
			// client to have the correct id.
			sender_.sendToAllExcept(remoteConnection, remoteConnection->getClientInfo());
			break;
		}
		default:
			remoteReceive(remoteConnection, packet);
			// Send through to all connections.
			sender_.sendToAllExcept(remoteConnection, packet);
			break;
	}
}

void TetrisGame::clientReceive(net::Packet& packet) {
	packet.reset();
	PacketType type;
	packet >> type;
	int id;
	packet >> id;
	auto remote = sender_.findRemoteConnection(id);

	switch (type) {
		case PacketType::CONNECTION_INFO:
		{
			// New connection?
			if (remote == nullptr) {
				// Add new connection.
				remote = sender_.addRemoteConnection(id, nullptr);
			}
			remoteReceive(remote, packet);
			break;
		}
		case PacketType::CONNECTION_ID:
			localConnection_.setId(id);
			break;
		default:
			remoteReceive(remote, packet);
			break;
	}
}

void TetrisGame::remoteReceive(std::shared_ptr<RemoteConnection> remoteConnection, net::Packet& packet) {
	packet.reset();
	PacketType type;
	packet >> type;
	int id;
	packet >> id;
	
	switch (type) {
		case PacketType::RESTART:
			if (pause_) {
				pause_ = false;
				eventHandler_(GamePause(pause_));
			}
			localConnection_.restart();
			initGame();
			break;
		case PacketType::PAUSE:
			pause_ = !pause_;
			eventHandler_(GamePause(pause_));
			break;
		case PacketType::BOARD_SIZE:
			pause_ = false;
			packet >> width_;
			packet >> height_;
			localConnection_.resizeAndSend(width_, height_);
			initGame();
			break;
		default:
			if (remoteConnection) {
				remoteConnection->receive(packet);
			} else {
				// Protocol error.
				throw 1;
			}
			break;
	}
}

void TetrisGame::applyRules(Player& player, GameEvent gameEvent) {
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
				for (auto& local : localConnection_) {
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

	player.setClearedRows(player.getClearedRows() + rows);

	try {
		auto localPlayer = dynamic_cast<LocalPlayer&>(player);
		// Rows removed from local player?
		if (rows != 0) {
			localPlayer.setPoints(localPlayer.getPoints() + localPlayer.getLevel() * rows * rows);

			// Multiplayer?
			if (nbrOfPlayers_ > 1) {
				// Increase level up counter for all opponents to the current player.
				// Remote players will be added indirectly.
				for (auto& opponent : localConnection_) {
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
	} catch (std::bad_cast&) {
		// Reference cast failed, is therefore a remote player. Do nothing!
	}
}

void TetrisGame::Sender::sendToAll(const net::Packet& packet) const {
	sendToAllExcept(nullptr, packet);
}

bool TetrisGame::Sender::isActive() const {
	return !remoteConnections_.empty() && clientConnection_ != nullptr;
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

std::shared_ptr<RemoteConnection> TetrisGame::Sender::addRemoteConnection(int connectionId, net::ConnectionPtr connection) {
	// Add the new connection.
	auto remote = std::make_shared<RemoteConnection>(connectionId, connection);
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
