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
	lastConnectionId_(UNDEFINED_CONNECTION_ID),
	network_(10),
	countDownTime_(COUNT_DOWN_TIME),
	timeLeftToStart_(-0.0),
	wholeTimeLeft_(0) {
}

TetrisGame::~TetrisGame() {
	closeGame();
}

void TetrisGame::resumeGame(int rows, int columns, const std::vector<PlayerData>& playersData) {
	width_ = columns;
	height_ = rows;
	status_ = LOCAL;
	localConnection_.removeAllPlayers();
	for (const PlayerData& data : playersData) {
		localConnection_.addPlayer(data.device_, width_, height_,data.levelUpCounter_,
			data.points_, data.level_, data.current_, data.next_, data.board_);
	}
	nbrOfPlayers_ = localConnection_.getNbrOfPlayers();
	initGame();
}

std::vector<PlayerData> TetrisGame::getPlayerData() const {
	std::vector<PlayerData> playerData;
	for (const std::shared_ptr<LocalPlayer>& player : localConnection_) {
		playerData.emplace_back();
		const TetrisBoard& tetrisBoard = player->getTetrisBoard();
		playerData.back().current_ = tetrisBoard.getBlock();
		playerData.back().next_ = tetrisBoard.getNextBlockType();
		playerData.back().board_ = tetrisBoard.getBoardVector();
		playerData.back().levelUpCounter_ = player->getLevelUpCounter();
		playerData.back().level_ = player->getLevel();
		playerData.back().name_ = player->getName();
		playerData.back().points_ = player->getPoints();
		playerData.back().device_ = player->getDevice();
	}

	return playerData;
}

void TetrisGame::createLocalGame() {
	if (status_ == WAITING_TO_CONNECT) {
		status_ = LOCAL;

		lastConnectionId_ = UNDEFINED_CONNECTION_ID;
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

		lastConnectionId_ = UNDEFINED_CONNECTION_ID;
		localConnection_.setId(lastConnectionId_);

		network_.startClient(ip, port);
	}
}

void TetrisGame::initGame() {
	// Signals the gui that the game begins.
	GameStart gameStart(GameStart::LOCAL);
	switch (status_) {
		case TetrisGame::CLIENT:
			gameStart = GameStart(GameStart::CLIENT);
			break;
		case TetrisGame::SERVER:
			gameStart = GameStart(GameStart::SERVER);
			break;
	}
	eventHandler_(gameStart);

	if (pause_) {
		pause_ = false;
		GamePause gamePause(pause_);
		eventHandler_(gamePause);
	}

	// Init all players.
	std::vector<std::shared_ptr<Player>> players;
	for (auto& player : localConnection_) {
		players.push_back(player);
	}
	std::vector<std::shared_ptr<Connection>> remoteConnections;
	for (auto& remoteConnection : sender_) {
		remoteConnections.push_back(remoteConnection);
		for (auto& player : *remoteConnection) {
			players.push_back(player);
		}
	}

	nbrOfPlayers_ = players.size();
	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	InitGame initGame(players, remoteConnections);
	eventHandler_(initGame);

	if (currentGameHasCountDown()) { // Must be called last, after all settings are defined for the current game.
		timeLeftToStart_ = countDownTime_;
		wholeTimeLeft_ = countDownTime_;
		CountDown countDown(wholeTimeLeft_);
		eventHandler_(countDown);
	}
}

void TetrisGame::restartGame() {
	if (status_ != Status::WAITING_TO_CONNECT) {
		if (network_.isServer() || network_.isClient()) {
			// Go from looby to game.
			net::Packet packet;
			packet << PacketType::RESTART;
			packet << localConnection_.getId();
			sender_.sendToAll(packet);
		}

		localConnection_.restart();
		initGame();
	}
}

// Stop the game and abort any active connection.
void TetrisGame::closeGame() {
	status_ = WAITING_TO_CONNECT;

	// Disconnecting.
	network_.stop();

	sender_.disconnect();
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
	GamePause gamePause(pause_);
	eventHandler_(gamePause);
	
	if (!pause_ && nbrOfPlayers_ > 1) {
		timeLeftToStart_ = countDownTime_;
		wholeTimeLeft_ = countDownTime_;
		CountDown countDown(wholeTimeLeft_);
		eventHandler_(countDown);
	}
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
		}
		localConnection_.resizeBoard(width, height);

		initGame();
	}
}

void TetrisGame::setPlayers(const std::vector<DevicePtr>& devices) {
	// Add human players.
	localConnection_.setPlayers(TETRIS_WIDTH, TETRIS_HEIGHT, devices);
	for (std::shared_ptr<LocalPlayer>& player : localConnection_) {
		player->addGameEventListener(std::bind(&TetrisGame::applyRulesForLocalPlayers, this, std::placeholders::_1, std::placeholders::_2, player));
	}

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
			packet << PacketType::BOARD_SIZE;
			packet << localConnection_.getId();
			packet << width_ << height_;
			connection->send(packet);

			// Send connection info of all connections to the new connection.
			connection->send(localConnection_.getClientInfo());
			for (auto& remote : sender_) {
				if (newRemote != remote) {
					newRemote->send(remote->getClientInfo());
				}
			}

			// Send the new connection info to the old connections.
			sender_.sendToAllExcept(newRemote, newRemote->getClientInfo());

			NewConnection newConnection;
			eventHandler_(newConnection);
		}

		// Remove the disconnected connections.
		sender_.serverRemoveDisconnectedConnections();

		for (auto& client : sender_) {
			net::Packet packet;
			while (client->pollReceivePacket(packet)) {
				serverReceive(client, packet);
			}
		}
	} else if (network_.isClient()) {
		if (!sender_.isActive()) {
			auto connectionToServer = network_.pollConnection();
			if (connectionToServer) {
				sender_.setServerConnection(connectionToServer);
				sender_.sendToAll(localConnection_.getClientInfo());
				NewConnection newConnection;
				eventHandler_(newConnection);
			}
		} else {
			net::Packet packet;
			while (sender_.receivePacketFromServer(packet)) {
				clientReceive(packet);
			}
		}
	}
}

void TetrisGame::update(double deltaTime) {
	if (status_ != Status::WAITING_TO_CONNECT) {
		receiveAndSendNetworkData();

		if (!pause_) {
			if (timeLeftToStart_ > 0) {
				timeLeftToStart_ -= deltaTime;
			}

			if (currentGameHasCountDown() && wholeTimeLeft_ != (int) (timeLeftToStart_ + 1)
				&& wholeTimeLeft_ > timeLeftToStart_ + 1) {

				wholeTimeLeft_ = (int) (timeLeftToStart_ + 1);
				CountDown countDown(wholeTimeLeft_);
				eventHandler_(countDown);
			}

			if (nbrOfPlayers_ == 1 || currentGameHasCountDown() && timeLeftToStart_ < 0) {
				// No count down for one player game.
				// Update the game.
				localConnection_.updateGame(deltaTime);
			}
		}
	}
}

void TetrisGame::serverReceive(std::shared_ptr<RemoteConnection> remoteConnection, net::Packet& packet) {
	packet.reset();
	PacketType type;
	packet >> type;
	packet[2] = remoteConnection->getId(); // Set the connection id. The remote has no obligation to use the correct id.

	// Send through to all connections.
	sender_.sendToAllExcept(remoteConnection, packet);

	// Receive the data and handle the data.
	remoteReceive(remoteConnection, packet);
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
			// New connection?
			if (remote == nullptr) {
				// Add new connection.
				remote = sender_.addRemoteConnection(id, nullptr);
			}
			remoteReceive(remote, packet);
			initGame();
			break;
		case PacketType::CONNECTION_DISCONNECT:
			sender_.removeConnection(id);
			initGame();
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
			localConnection_.restart();
			initGame();
			break;
		case PacketType::PAUSE:
		{
			pause_ = !pause_;
			GamePause gamePause(pause_);
			eventHandler_(gamePause);
			break;
		}
		case PacketType::BOARD_SIZE:
			packet >> width_;
			packet >> height_;
			localConnection_.resizeBoard(width_, height_);
			initGame();
			break;
		case PacketType::CONNECTION_INFO:
			remoteConnection->receive(packet);
			for (std::shared_ptr<RemotePlayer>& player : *remoteConnection) {
				player->addGameEventListener(std::bind(&TetrisGame::applyRulesForRemotePlayers, this, std::placeholders::_1, std::placeholders::_2, player));
			}
			break;
		case PacketType::CONNECTION_START_BLOCK:
			initGame();
			// Fall through.
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

void TetrisGame::applyRulesForLocalPlayers(GameEvent gameEvent, const TetrisBoard& board, std::shared_ptr<LocalPlayer>& player) {
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
					if (player->getId() != local->getId()) {
						std::vector<BlockType> blockTypes;
						for (int i = 0; i < rows; ++i) {
							std::vector<BlockType> tmp = generateRow(local->getTetrisBoard(), 0.79);
							blockTypes.insert(blockTypes.begin(), tmp.begin(), tmp.end());
						}
						local->addExternalRows(blockTypes);
						if (sender_.isActive()) {
							net::Packet packet;
							packet << PacketType::PLAYER_TETRIS;
							packet << localConnection_.getId();
							packet << local->getId();
							for (auto blockType : blockTypes) {
								packet << localConnection_.getId();
							}
							sender_.sendToAll(packet);
						}
					}
				}
			}
			break;
		case GameEvent::GAME_OVER:
			// One player more is dead.
			--nbrOfAlivePlayers_;

			GameOver gameOver(nbrOfAlivePlayers_ + 1, player);
			eventHandler_(gameOver);

			// Multiplayer?
			if (nbrOfPlayers_ > 1) {
				// All dead except one => End game!
				if (nbrOfAlivePlayers_ == 1) {
					for (auto& local : localConnection_) {
						local->endGame();
					}
				}
			}
			break;
	}

	if (rows != 0) {
		int newPoints = player->getPoints() + player->getLevel() * rows * rows;
		PointsChange pointsChange(player, player->getPoints(), newPoints);
		player->setPoints(newPoints);
		eventHandler_(pointsChange);

		// Multiplayer?
		if (nbrOfPlayers_ > 1) {
			// Increase level up counter for all opponents to the current player.
			// Remote players will be added indirectly.
			for (auto& opponent : localConnection_) {
				if (opponent->getId() != player->getId()) {
					opponent->setLevelUpCounter(opponent->getLevelUpCounter() + rows);
				}
			}
		} else { // Singleplayer!
			player->setLevelUpCounter(player->getLevelUpCounter() + rows);
		}

		// Set level.
		int level = (player->getLevelUpCounter() / ROWS_TO_LEVEL_UP) + 1;
		if (level <= maxLevel_ && level != player->getLevel()) {
			int oldLevel = player->getLevel();
			player->setLevel(level);
			LevelChange levelChange(player, level, oldLevel);
			eventHandler_(levelChange);
		}
	}
}

void TetrisGame::applyRulesForRemotePlayers(GameEvent gameEvent, const TetrisBoard& board, std::shared_ptr<RemotePlayer>& player) {
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
			break;
		case GameEvent::GAME_OVER:
		{
			GameOver gameOver(nbrOfAlivePlayers_--, player);
			eventHandler_(gameOver);
			break;
		}
	}

	if (nbrOfPlayers_ > 1 && rows > 0) {
		// Increase level up counter for all opponents to the current player.
		// Remote players will be added indirectly.
		for (auto& opponent : localConnection_) {
			if (opponent->getId() != player->getId() && !opponent->getTetrisBoard().isGameOver()) {
				opponent->setLevelUpCounter(opponent->getLevelUpCounter() + rows);
			}
		}
	}
}

void TetrisGame::Sender::sendToAll(const net::Packet& packet) const {
	sendToAllExcept(nullptr, packet);
}

bool TetrisGame::Sender::isActive() const {
	return !remoteConnections_.empty() || connectionToServer_ != nullptr && connectionToServer_->isActive();
}

void TetrisGame::Sender::sendToAllExcept(std::shared_ptr<RemoteConnection> remoteSendNot, const net::Packet& packet) const {
	if (connectionToServer_) {
		connectionToServer_->send(packet);
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

void TetrisGame::Sender::serverRemoveDisconnectedConnections() {
	// Find iterator to the connections that were disconnected.
	auto newEnd = std::remove_if(remoteConnections_.begin(), remoteConnections_.end(),
		[&](const std::shared_ptr<RemoteConnection>& remote) {
		if (!remote->isActive()) {
			// Signal all connections that one connection has disconnected.
			net::Packet packet;
			packet << PacketType::CONNECTION_DISCONNECT;
			packet << remote->getId();
			sendToAllExcept(remote, packet);
			return true;
		}
		return false;
	});
	// Remove all connections that were disconnected.
	remoteConnections_.erase(newEnd, remoteConnections_.end());
}

void TetrisGame::Sender::disconnect() {
	remoteConnections_.clear();
	connectionToServer_ = nullptr;
}

void TetrisGame::Sender::removeConnection(int connectionId) {
	auto it = std::find_if(remoteConnections_.begin(), remoteConnections_.end(),
		[connectionId](const std::shared_ptr<RemoteConnection>& remote) {

		return remote->getId() == connectionId;
	});

	if (it != remoteConnections_.end()) {
		remoteConnections_.erase(it);
	}
}
