#include "tetrisgame.h"

#include "tetrisboard.h"
#include "player.h"
#include "remoteplayer.h"
#include "userconnection.h"
#include "localplayer.h"
#include "networkevent.h"
#include "userconnection.h"
#include "localplayer.h"
#include "computer.h"
#include "device.h"
#include "tetrisparameters.h"

#include <mw/exception.h>
#include <mw/packet.h>
#include <mw/server.h>
#include <mw/enetclient.h>
#include <mw/enetserver.h>
#include <mw/localnetwork.h>

#include <vector>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <memory>

namespace {

	// Defines the packet content.
	// Value of first byte by a client.
	enum class PacketType : char {
		INPUT,       // Tetrisboard updates.
		STARTGAME,   // The server starts the game. All user starts the game.
		READY,       // The client is ready/unready to start.
		SERVERINFO,  // Sent from the server. The info about players and tetrisboard conditions (e.g. length and width).
		TETRIS,      // Data describing when player adds rows.
		CLIENTINFO,  // A client send info to the server.
		STARTBLOCK,  // Sends the start current block and the next block.
		PAUSE        // Pause/Unpause the game for all users.
	};

	mw::Packet& operator<<(mw::Packet& packet, const Input& input) {
		char data = input.rotate_;
		data <<= 1;
		data += input.down_;
		data <<= 1;
		data += input.left_;
		data <<= 1;
		data += input.right_;
		packet << data;
		return packet;
	}

	mw::Packet& operator>>(mw::Packet& packet, Input& input) {
		char data;
		packet >> data;
		char bit = 1;
		input.right_ = (bit & data) > 0;
		bit <<= 1;
		input.left_ = (bit & data) > 0;
		bit <<= 1;
		input.down_ = (bit & data) > 0;
		bit <<= 1;
		input.rotate_ = (bit & data) > 0;
		return packet;
	}

	mw::Packet& operator<<(mw::Packet& packet, PacketType type) {
		packet << static_cast<char>(type);
		return packet;
	}

	mw::Packet& operator>>(mw::Packet& packet, PacketType& type) {
		char tmp;
		packet >> tmp;
		type = static_cast<PacketType>(tmp);
		return packet;
	}

	mw::Packet& operator<<(mw::Packet& packet, Move move) {
		packet << static_cast<char>(move);
		return packet;
	}

	mw::Packet& operator>>(mw::Packet& packet, Move& move) {
		char tmp;
		packet >> tmp;
		move = static_cast<Move>(tmp);
		return packet;
	}

	mw::Packet& operator<<(mw::Packet& packet, BlockType type) {
		packet << static_cast<char>(type);
		return packet;
	}

	mw::Packet& operator>>(mw::Packet& packet, BlockType& type) {
		char tmp;
		packet >> tmp;
		type = static_cast<BlockType>(tmp);
		return packet;
	}

	// char type = STARTGAME
	void serverSendStartGame(mw::Server* server) {
		mw::Packet data;
		data << PacketType::STARTGAME;
		server->serverPushToSendBuffer(data, mw::Network::RELIABLE);
	}

} // Anonymous namespace.

TetrisGame::TetrisGame() {
	pause_ = false;
	serverPort_ = 12008;
	connectToPort_ = 12008;
	start_ = false;
	connectToIp_ = "localhost";
	network_ = nullptr;
	server_ = nullptr;
	acceptNewConnections_ = false;
	status_ = WAITING_TO_CONNECT;
	nbrOfPlayers_ = 0;
	playerId_ = 0;
	localUser_ = UserConnectionPtr(new UserConnection(0));

	timeStep_ = 17; // Fix time step.
	accumulator_ = 0; // Time accumulator.
	countDown_ = 3000;
}

TetrisGame::~TetrisGame() {
	closeGame();
	delete network_;
}

void TetrisGame::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers) {
	connect(devices, nbrOfComputers, LOCAL);
}

void TetrisGame::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int width, int height, int maxLevel) {
	width_ = width;
	height_ = height;
	maxLevel_ = maxLevel;
	connect(devices, nbrOfComputers, LOCAL);
}

void TetrisGame::createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int port, int width, int height, int maxLevel) {
	width_ = width;
	height_ = height;
	maxLevel_ = maxLevel;
	setServerPort(port);
	connect(devices, nbrOfComputers, SERVER);
}

void TetrisGame::createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
	int port, std::string ip, int maxLevel) {
	if (status_ == WAITING_TO_CONNECT) {
		setConnectToIp(ip);
		setConnectToPort(port);
		connect(devices, nbrOfComputers, CLIENT);
	}
}

void TetrisGame::startGame() {
	// Game is not started? And this is a server? And network is active?
	if (!start_ && server_ != nullptr && network_->getStatus() == mw::Network::ACTIVE) {
		// Only the server can start the game!
		serverSendStartGame(server_);
	}
}

void TetrisGame::restartGame() {
	// Game is started? And this is a server?
	if (start_ && server_ != nullptr) {
		// Only the server can restart the game!
		serverSendStartGame(server_);
	}
}

// Stops the game and aborts any active connection.
void TetrisGame::closeGame() {
	if (network_ != 0) {
		start_ = false;
		localUser_->setReady(false);
		status_ = WAITING_TO_CONNECT;

		// Disconnecting
		network_->stop();

		users_.clear();
		localUser_->clear();
	}
}

bool TetrisGame::isPaused() const {
	return pause_;
}

void TetrisGame::pause() {
	if (network_ != 0 && isStarted()) {
		sendPause();
	}
}

void TetrisGame::changeReadyState() {
	if (network_ != 0 && !isStarted()) {
		sendReady();
	}
}

bool TetrisGame::isReady() const {
	return localUser_->isReady();
}

bool TetrisGame::isStarted() const {
	return start_;
}

void TetrisGame::addCallback(mw::Signal<NetworkEventPtr>::Callback callback) {
	eventHandler_.connect(callback);
}

int TetrisGame::getNbrOfPlayers() const {
	return nbrOfPlayers_;
}

void TetrisGame::update(Uint32 deltaTime) {
	if (network_ != 0) {
		switch (network_->getStatus()) {
			case mw::Network::NOT_ACTIVE:
				break;
			case mw::Network::ACTIVE:
			{
				mw::Packet data;
				int id = 0;
				while ((id = network_->pullFromReceiveBuffer(data)) != 0) {
					receiveData(data, id);
				}
			}
				if (isStarted()) {
					if (!pause_) {
						countDown_ -= deltaTime;
						if (countDown_ < 0) {
							if (countDown_ > -20000) { // -20000 = Time impossible to reach.
								gameHandler_->countDown(countDown_);
							}
							updateGame(deltaTime);
							countDown_ = -20000;
						} else {
							gameHandler_->countDown(countDown_);
						}
					}

					// Update the board for the local users.
					for (PlayerPtr& player : *localUser_) {
						Move move;
						BlockType next;
						// Update the local player tetris board and send the input to all remote players.
						while (player->updateBoard(move, next)) {
							sendInput(player->getId(), move, next);
						}
					}
				}
				break;
			case mw::Network::DISCONNECTING:
				// Will soon change status to not active.
				break;
		}
	} else {
		status_ = WAITING_TO_CONNECT;
	}
}

// Initiates the choosen connection.
void TetrisGame::connect(const std::vector<DevicePtr>& devices, int nbrOfComputerPlayers, Status status) {
	if (status_ == WAITING_TO_CONNECT) {
		// Settings.
		delete network_;
		network_ = nullptr;
		server_ = nullptr;
		status_ = status;
		acceptNewConnections_ = true;
		devices_ = devices;
		nbrOfPlayers_ = 0;
		playerId_ = 0;

		// Add computer players.
		for (int i = 0; i < nbrOfComputerPlayers; ++i) {
			devices_.push_back(DevicePtr(new Computer(ais_[i])));
		}

		nbrOfPlayers_ = devices_.size();

		switch (status) {
			case WAITING_TO_CONNECT:
				return;
			case LOCAL:
			{
				mw::LocalNetwork* server = new mw::LocalNetwork(*this);
				network_ = server;
				server_ = server;
			}
				network_->start();

				localUser_ = UserConnectionPtr(new UserConnection(network_->getId()));
				localUser_->setReady(true);
				users_.push_back(localUser_);
				// Add new player to all local players.
				for (const DevicePtr& device : devices_) {
					localUser_->add(PlayerPtr(new LocalPlayer(++playerId_, width_, height_, device)));
				}
				break;
			case SERVER:
			{
				mw::EnetServer* server = new mw::EnetServer(serverPort_, *this);
				network_ = server;
				server_ = server;
			}
				network_->start();

				localUser_ = UserConnectionPtr(new UserConnection(network_->getId()));
				users_.push_back(localUser_);
				// Add new player to all local players.
				for (const DevicePtr& device : devices_) {
					localUser_->add(PlayerPtr(new LocalPlayer(++playerId_, width_, height_, device)));
				}
				{
					auto newConnection = std::make_shared<NewConnection>(NewConnection::SERVER);
					newConnection->add(network_->getId(), localUser_->getNbrOfPlayers(), localUser_->isReady());
					eventHandler_(newConnection);
				}
				break;
			case CLIENT:
				network_ = new mw::EnetClient(connectToPort_, connectToIp_);
				network_->start();
				sendClientInfo();
				break;
		};
	}
}

void TetrisGame::receiveToServer(const mw::Packet& packet, int clientId) {
	PacketType type = static_cast<PacketType>(packet[0]);
	switch (type) {
		case PacketType::SERVERINFO:
			// Game started?
			if (start_) {
				throw mw::Exception("Protocol error!\n");
			}

			// This is not a server?
			if (server_ != 0) {
				// Only real clients (i.e. not server).
				clientReceiveServerInfo(packet);
			}
			break;
		case PacketType::CLIENTINFO:
		{
			// Game started?
			if (start_) {
				throw mw::Exception("Protocol error!\n");
			}

			// Find the remote user with id (fromId).
			auto it = std::find_if(users_.begin(), users_.end(), [clientId](UserConnectionPtr remote) {
				return remote->getId() == clientId;
			});

			// Find remote player.
			UserConnectionPtr remote = *it;

			// Failed to find?
			if (remote == nullptr) {
				throw mw::Exception("Protocol error!\n");
			}

			serverReceiveClientInfo(remote, packet);

			sendServerInfo(); // Is sent before the data will be sent.
			break;
		}
		case PacketType::STARTGAME:
			// Data not from server?
			if (mw::Network::SERVER_ID != clientId) {
				throw mw::Exception("Protocol error!\n");
			}

			// Check if all players ready to start!
			if (!isAllUsersReady()) {
				// Not ready -> don't start!
				break;
			}

			// Stops new connections.
			acceptNewConnections_ = false;

			// This server will start also.
			clientStartGame();

			// Sent to all players to start the game.
			break;
		default:
			break;
	}
}

bool TetrisGame::connectToServer(int clientId) {
	if (acceptNewConnections_) {
		users_.push_back(UserConnectionPtr(new UserConnection(clientId)));

		sendServerInfo();

		// Accept connection!
		return true;
	}
	// Refuse connection!
	return false;
}

void TetrisGame::disconnectToServer(int clientId) {
	auto it = users_.begin();
	for (; it != users_.end(); ++it) {
		UserConnectionPtr tmp = *it;
		if (tmp->getId() == clientId) {
			break;
		}
	}
	// Not started? Or has it ended?
	if (!start_) {
		if (it != users_.end()) {
			users_.erase(it);
			sendServerInfo();
		}
	} else {
		// Game is started. Set offline players to gameOver.
		UserConnectionPtr tmp = *it;
		for (PlayerPtr player : *tmp) {
			//player->triggerGameOverEvent();
		}
	}
}


// Receives data from a client or server.
// First element must be a value defined in PacketType.
void TetrisGame::receiveData(const mw::Packet& data, int id) {
	PacketType type = static_cast<PacketType>(data[0]);
	switch (type) {
		case PacketType::INPUT:
		{
			// Game not started?
			if (!start_) {
				throw mw::Exception("Protocol error!\n");
			}

			if (data.size() != 4) {
				throw mw::Exception("Protocol error!\n");
			}

			char playerId;
			Move move;
			BlockType next;
			receivInput(data, playerId, move, next);

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
			break;
		}
		case PacketType::READY:
			// Game started?
			if (start_) {
				// Don't change anything, break!
				break;
			}

			// Finds user and change the ready state.
			iterateUserConnections([&](UserConnection& user) {
				if (user.getId() == id) {
					user.setReady(!user.isReady());
					eventHandler_(std::make_shared<GameReady>(user.getId(), user.isReady()));
					return false; // User found, stop looking!
				}
				// User not found, keep iterating!!
				return true;
			});
			break;
		case PacketType::TETRIS:
		{
			// Game not started?
			if (!start_) {
				throw mw::Exception("Protocol error!\n");
			}

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

			// Add rows to the player with correct id.
			iterateAllPlayers([&](PlayerPtr player) {
				bool findPlayer = false;
				if (player->getId() == playerId) {
					player->update(blockTypes);
					findPlayer = true;
				}
				return !findPlayer;
			});
			break;
		}
		case PacketType::STARTGAME:
			// Signals the game to start. Only sent by the server.

			// Is not sended by server?
			if (mw::Network::SERVER_ID != id) {
				throw mw::Exception("Protocol error!\n");
			}

			clientStartGame();

			countDown_ = 3000;
			break;
		case PacketType::PAUSE:
			if (data.size() != 1) {
				throw mw::Exception("Protocol error!\n");
			}
			pause_ = !pause_;

			// Signals the gui that the game begins.
			eventHandler_(std::make_shared<GamePause>(pause_));

			countDown_ = 3000;
			break;
		case PacketType::STARTBLOCK:
			// Remote user?
			if (network_->getId() != id) {
				receiveStartBlock(data, id);
			}
			break;
		case PacketType::CLIENTINFO:
			// Sent by yourself to the server. Do nothing.
			break;
		default:
			throw mw::Exception("Protocol error!\n");
			break;
	}
}

void TetrisGame::iterateAllPlayers(std::function<bool(PlayerPtr)> nextPlayer) const {
	for (UserConnectionPtr user : users_) {
		bool next = true;
		for (PlayerPtr player : *user) {
			next = nextPlayer(player);
			if (!next) {
				break;
			}
		}
		if (!next) {
			break;
		}
	}
}

void TetrisGame::iterateUserConnections(std::function<bool(const UserConnection&)> nextUserConnection) const {
	for (const UserConnectionPtr& user : users_) {
		bool next = nextUserConnection(*user);
		if (!next) {
			break;
		}
	}
}

void TetrisGame::iterateUserConnections(std::function<bool(UserConnection&)> nextUserConnection) {
	for (UserConnectionPtr& user : users_) {
		bool next = nextUserConnection(*user);
		if (!next) {
			break;
		}
	}
}

bool TetrisGame::isAllUsersReady() {
	bool allUsersReady = true;
	iterateUserConnections([&](UserConnection& user) {
		if (user.isReady()) {
			return true;
		}
		allUsersReady = false;
		return false;
	});

	return allUsersReady;
}

void TetrisGame::serverReceiveClientInfo(UserConnectionPtr remote, mw::Packet packet) {
	PacketType type;
	packet >> type;
	char nbrOfPlayers;
	packet >> nbrOfPlayers;

	// Number of players to add or remove.
	remote->clear();

	for (int i = 0; i < nbrOfPlayers; ++i) {
		PlayerPtr player(new RemotePlayer(++playerId_, width_, height_));
		remote->add(player);
		char length;
		packet >> length;
		std::stringstream stream;
		for (int j = 0; j < length; ++j) {
			char chr;
			packet >> chr;
			stream << chr;
		}
		player->setName(stream.str());
	}
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
	auto newConnection = std::make_shared<NewConnection>(NewConnection::SERVER);
	nbrOfPlayers_ = 0;
	// Add new player to all human players.
	mw::Packet data;
	data << PacketType::SERVERINFO;
	data << width_ << height_;
	iterateUserConnections([&](const UserConnection& user) {
		data << user.getId();
		data << user.isReady();
		data << user.getNbrOfPlayers();
		nbrOfPlayers_ += user.getNbrOfPlayers();
		newConnection->add(user.getId(), user.getNbrOfPlayers(), user.isReady());
		for (PlayerPtr player : user) {
			data << player->getId();
			std::string name = player->getName();
			data << name.length();
			for (char chr : name) {
				data << chr;
			}
		}
		return true;
	});

	eventHandler_(newConnection);
	server_->serverPushToSendBuffer(data, mw::Network::RELIABLE);
}

void TetrisGame::clientReceiveServerInfo(mw::Packet data) {
	if (data.size() < 3) {
		throw mw::Exception("Protocol error!\n");
	}

	users_.clear();
	nbrOfPlayers_ = 0;
	auto newConnection = std::make_shared<NewConnection>(NewConnection::CLIENT);
	width_ = data[1];
	height_ = data[2];

	int index = 2;
	while (++index < data.size()) {
		int id = data[index];
		bool ready = data[++index] != 0;
		int nbrOfPlayers = data[++index];
		nbrOfPlayers_ += nbrOfPlayers;

		newConnection->add(id, nbrOfPlayers, ready);

		// This network (local)?
		if (id == network_->getId()) {
			localUser_ = UserConnectionPtr(new UserConnection(id));
			users_.push_back(localUser_);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				PlayerPtr player(new LocalPlayer(playerId, width_, height_, devices_[i]));
				localUser_->add(player);
				localUser_->setReady(ready);
				std::stringstream stream;
				int length = data[++index];
				for (int i = 0; i < length; ++i) {
					stream << data[++index];
				}
				player->setName(stream.str());
			}
		} else {
			UserConnectionPtr user(new UserConnection(id));
			users_.push_back(user);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				PlayerPtr player(new RemotePlayer(playerId, width_, height_));
				user->add(player);
				user->setReady(ready);
				std::stringstream stream;
				int length = data[++index];
				for (int i = 0; i < length; ++i) {
					stream << data[++index];
				}
				player->setName(stream.str());
			}
		}
	}

	eventHandler_(newConnection);
}

// char type = CLIENTINFO
// char nbrOfPlayers
void TetrisGame::sendClientInfo() {
	mw::Packet data;
	data << PacketType::CLIENTINFO;
	data << devices_.size();
	for (auto& device : devices_) {
		std::string name = device->getPlayerName();
		data << name.length();
		for (char chr : name) {
			data << chr;
		}
	}
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
}

void TetrisGame::sendReady() {
	mw::Packet data;
	data << PacketType::READY;
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
}

// char type = INPUT
// char playerId
// char move
// char nextBlockType, before move has taken effect
void TetrisGame::sendInput(char playerId, Move move, BlockType next) {
	mw::Packet data;
	data << PacketType::INPUT;
	data << playerId;
	data << move;
	data << next;
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
}

void TetrisGame::receivInput(mw::Packet packet, char& playerId, Move& move, BlockType& next) {
	PacketType type;
	packet >> type;
	packet >> playerId;
	packet >> move;
	packet >> next;
}

void TetrisGame::sendStartBlock() {
	mw::Packet data;
	data << PacketType::STARTBLOCK;
	for (PlayerPtr player : *localUser_) {
		data << player->getCurrentBlock();
		data << player->getNextBlock();
	}
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
}

void TetrisGame::receiveStartBlock(const mw::Packet& data, int id) {
	UserConnectionPtr user = nullptr;
	for (UserConnectionPtr tmp : users_) {
		if (tmp->getId() == id) {
			user = tmp;
			break;
		}
	}

	if (user == nullptr) {
		throw mw::Exception("Protocol error!\n");
	}

	auto it = user->begin();
	int i = 0;
	while (++i < data.size()) {
		BlockType current = static_cast<BlockType>(data[i]);
		BlockType next = static_cast<BlockType>(data[++i]);

		// Player exist?
		if (it != user->end()) {
			PlayerPtr player = *it;
			player->restart();
			player->update(current, next);
		} else {
			throw mw::Exception("Protocol error!\n");
		}
		++it;
	}
}

void TetrisGame::sendTetrisInfo(char playerId, const std::vector<BlockType>& blockTypes) {
	mw::Packet data;
	data << PacketType::TETRIS;
	data << playerId;
	for (BlockType type : blockTypes) {
		data << type;
	}
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
}

void TetrisGame::sendPause() {
	mw::Packet data;
	data << PacketType::PAUSE;
	network_->pushToSendBuffer(data, mw::Network::RELIABLE);
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

void TetrisGame::clientStartGame() {
	// Game already started?
	if (start_) {
		// Restart local players.
		for (PlayerPtr player : *localUser_) {
			// Restart player.
			player->restart();
		}
	} else {
		// Signals the gui that the game begins.
		switch (status_) {
			case TetrisGame::LOCAL:
				eventHandler_(std::make_shared<GameStart>(GameStart::LOCAL));
				break;
			case TetrisGame::CLIENT:
				eventHandler_(std::make_shared<GameStart>(GameStart::CLIENT));
				break;
			case TetrisGame::SERVER:
				eventHandler_(std::make_shared<GameStart>(GameStart::SERVER));
				break;
			case TetrisGame::WAITING_TO_CONNECT:
				break;
		}
	}

	start_ = true;
	pause_ = false;
	// When server decides to start all clients must be ready.
	// If not they are set to ready. To avoid sync problem.
	iterateUserConnections([&](UserConnection& user) {
		user.setReady(true);
		return true;
	});

	sendStartBlock();
	std::vector<PlayerPtr> players;
	iterateAllPlayers([&players](PlayerPtr player) {
		players.push_back(player);
		player->setPlayerInfo(PlayerInfo());
		return true;
	});
	
	nbrOfAlivePlayers_ = nbrOfPlayers_; // All players are living again.
	gameHandler_->initGame(players);

	// Signals the gui that the game is not paused.
	eventHandler_(std::make_shared<GamePause>(pause_));
}

void TetrisGame::updateGame(Uint32 msDeltaTime) {
	// DeltaTime to big?
	if (msDeltaTime > 250) {
		// To avoid spiral of death.
		msDeltaTime = 250;
	}

	accumulator_ += msDeltaTime;
	while (accumulator_ >= timeStep_) {
		accumulator_ -= timeStep_;

		iterateAllPlayers([&](PlayerPtr player) {
			player->update(timeStep_ / 1000.0);
			GameEvent gameEvent;
			while (player->pollGameEvent(gameEvent)) {
				applyRules(*player, gameEvent);
				gameHandler_->eventHandler(player, gameEvent);
			}
			return true;
		});
	}
}

void TetrisGame::applyRules(Player& player, GameEvent gameEvent) {
	PlayerInfo& pInfo = player.getPlayerInfo();
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
				for (PlayerPtr local : *localUser_) {
					if (player.getId() == local->getId()) {
						std::vector<BlockType> blockTypes;
						for (int i = 0; i < rows; ++i) {
							std::vector<BlockType> tmp = local->generateRow();
							blockTypes.insert(blockTypes.begin(), tmp.begin(), tmp.end());
						}
						sendTetrisInfo(local->getId(), blockTypes);
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
					iterateAllPlayers([&](PlayerPtr player) {
						// Will be noticed in the next call to PlayerManager::applyRules(...).
						// Nothing happens if player allready dead.
						player->update(Move::GAME_OVER);
						return true;
					});
				}
			} else { // Singleplayer.
				// And is the correct settings?				
				if (player.getTetrisBoard().getNbrOfRows() == TETRIS_HEIGHT
					&& player.getTetrisBoard().getNbrOfColumns() == TETRIS_WIDTH
					&& maxLevel_ == TETRIS_MAX_LEVEL) {

					// Is local and a human player?
					if (status_ == LOCAL && !player.isAi()) {
						eventHandler_(std::make_shared<GameOver>(pInfo.points_));
					}
				}
			}
			break;
		default:
			break;
	}

	if (rows != 0) {
		// Assign points and number of cleared rows.
		pInfo.nbrClearedRows_ += rows;
		pInfo.points_ += player.getLevel() * rows * rows;

		// Multiplayer?
		if (nbrOfPlayers_ > 1) {
			// Increase level up counter for all opponents to the current player.
			iterateAllPlayers([&](PlayerPtr opponent) {
				if (opponent->getId() != player.getId()) {
					PlayerInfo& tmpInfo = opponent->getPlayerInfo();
					tmpInfo.levelUpCounter_ += rows;
				}
				return true;
			});
		} else { // Singleplayer!
			pInfo.levelUpCounter_ += rows;
		}

		// Set level.
		int level = (pInfo.levelUpCounter_ / ROWS_TO_LEVEL_UP) + 1;
		if (level <= maxLevel_) {
			player.setLevel(level);
		}
	}
}
