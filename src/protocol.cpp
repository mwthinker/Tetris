#include "protocol.h"

#include "tetrisboard.h"
#include "player.h"
#include "remoteplayer.h"
#include "userconnection.h"

#include <mw/packet.h>
#include <mw/enet/server.h>
#include <mw/enet/client.h>
#include <mw/localnetwork.h>

#include <algorithm>
#include <iostream>

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

Protocol::Protocol() {
	pause_ = false;
	serverPort_ = 12008;
	connectToPort_ = 12008;
	start_ = false;
	connectToIp_ = "localhost";
	network_ = nullptr;
	acceptNewConnections_ = false;
	status_ = WAITING_TO_CONNECT;
	nbrOfPlayers_ = 0;
	playerId_ = 0;
	localUser_ = UserConnectionPtr(new UserConnection(0));
}

Protocol::~Protocol() {
	closeGame();
	delete network_;
}

void Protocol::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers) {
	connect(devices, nbrOfComputers, LOCAL);
}

void Protocol::createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int width, int height, int maxLevel) {
    width_ = width;
	height_ = height;
	maxLevel_ = maxLevel;
	connect(devices, nbrOfComputers, LOCAL);
}

void Protocol::createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, int width, int height, int maxLevel) {
	width_ = width;
	height_ = height;
	maxLevel_ = maxLevel;
	setServerPort(port);
	connect(devices, nbrOfComputers, SERVER);
}

void Protocol::createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, std::string ip, int maxLevel) {
	if (status_ == WAITING_TO_CONNECT) {
		setConnectToIp(ip);
		setConnectToPort(port);
		connect(devices, nbrOfComputers, CLIENT);
	}
}

void Protocol::startGame() {
	// And game is not started? And network is not null? And network is active and this is a server?
	if (!isStarted() && network_ != 0 && network_->getStatus() == mw::Network::ACTIVE && network_->getId() == network_->getServerId()) {
		serverSendStartGame();
	}
}

void Protocol::restartGame() {
	// Network is not null? And this is a server? And game is started?
	if (network_ != 0 && network_->getId() == network_->getServerId() && isStarted()) {
		serverSendStartGame();
	}
}

// Stops the game and aborts any active connection.
void Protocol::closeGame() {
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

bool Protocol::isPaused() const {
	return pause_;
}

void Protocol::pause() {
	if (network_ != 0 && isStarted()) {
		sendPause();
	}
}

void Protocol::changeReadyState() {
	if (network_ != 0 && !isStarted()) {
		sendReady();
	}
}

bool Protocol::isReady() const {
	return localUser_->isReady();
}

bool Protocol::isStarted() const {
	return start_;
}

void Protocol::addCallback(mw::Signal<NetworkEventPtr>::Callback callback) {
	eventHandler_.connect(callback);
}

int Protocol::getNbrOfPlayers() const {
	return nbrOfPlayers_;
}

void Protocol::signalEvent(NetworkEventPtr nEvent) {
	eventHandler_(nEvent);
}

void Protocol::update(Uint32 deltaTime) {
	if (network_ != 0) {
		network_->update();
		switch (network_->getStatus()) {
		case mw::Network::NOT_ACTIVE:
			break;
		case mw::Network::ACTIVE:
			{
				mw::Packet data;
				int id = 0;
				while ( (id = network_->pullFromReceiveBuffer(data)) != 0 ) {
					receiveData(data, id);
				}
			}
			if (isStarted()) {
				for (PlayerPtr& player : *localUser_) {
					Move move;
					BlockType next;
					// Update the local player tetris board and send the input to all remote players.
					while (player->updateBoard(move,next)) {
						sendInput(player->getId(),move,next);
					}
				}

				if (!pause_) {
					updateGame(deltaTime);
				}
			}
			break;
		case mw::Network::DISCONNECTING:
			// Will soon change status to not active.
			std::cout << "\nDISCONNECTING: " << std::endl;
			break;
		}
	} else {
		status_ = WAITING_TO_CONNECT;
	}
}

// Initiates the choosen connection.
void Protocol::connect(const std::vector<DevicePtr>& devices, int nbrOfComputerPlayers, Status status) {
	if (status_ == WAITING_TO_CONNECT) {
		// Settings.
		delete network_;
		network_ = nullptr;
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
			break;
		case LOCAL:
			network_ = new mw::LocalNetwork(this);
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
			network_ = new mw::enet::Server(serverPort_,this);
			network_->start();

			localUser_ = UserConnectionPtr(new UserConnection(network_->getId()));
			users_.push_back(localUser_);
			// Add new player to all local players.
			for (const DevicePtr& device : devices_) {
				localUser_->add(PlayerPtr(new LocalPlayer(++playerId_, width_, height_, device)));
			}
			{
				auto newConnection = std::make_shared<NewConnection>(NewConnection::SERVER);
				newConnection->add(network_->getId(),localUser_->getNbrOfPlayers(),localUser_->isReady());
				signalEvent(newConnection);
			}
			break;
		case CLIENT:
			network_ = new mw::enet::Client(connectToPort_,connectToIp_);
			network_->start();
			sendClientInfo();
			break;
		};
	}
}

// @Override ServerFilter. Is only called in server/local mode.
// Data (data) is received from client (id). Type (type)
// describes the type of event. The return value is the
// data which will be sent to all clients.
bool Protocol::sendThrough(const mw::Packet& packet, int fromId, int toId, Type type) {
	switch (type) {
	case mw::ServerFilter::NEW_CONNECTION:
		if (acceptNewConnections_) {
			users_.push_back(UserConnectionPtr(new UserConnection(fromId)));

			sendServerInfo();
			std::cout << "\nNEW_CONNECTION" << packet.size() << std::endl;

			// Accept connection!
			return true;
		}
		// Refuse connection!
		return false;
	case mw::ServerFilter::PACKET:
		{
			PacketType type = static_cast<PacketType>(packet[0]);
			switch (type) {
			case PacketType::CLIENTINFO:
				{
					std::cout << "\nCLIENTINFO" << std::endl;

					// Game started?
					if (start_) {
						throw ProtocolError();
					}

					// Find the remote user with id (fromId).
					auto it = std::find_if(users_.begin(),users_.end(), [fromId] (UserConnectionPtr remote) {
						return remote->getId() == fromId;
					});

					// Find remote player.
					UserConnectionPtr remote = *it;

					// Failed to find?
					if (remote == nullptr) {
						throw ProtocolError();
					}

					serverReceiveClientInfo(remote,packet);

					sendServerInfo(); // Is sent before the data will be sent.
				}
				// Do not send to clients.
				return false;
			case PacketType::SERVERINFO:
				// Data not from server?
				if (network_->getServerId() != fromId) {
					throw ProtocolError();
				}

				return true;
			case PacketType::STARTGAME:
				// Data not from server?
				if (network_->getServerId() != fromId) {
					throw ProtocolError();
				}

				// Check if all players ready to start!
				if (!isAllUsersReady()) {
					// Not ready -> don't start!
					return false;;
				}

				// Stops new connections.
				acceptNewConnections_ = false;

				// This server will start also.
				clientStartGame();

				// Sent to all players to start the game.
				return true;
			default:
				return true;
			}
		}
		break;
	case mw::ServerFilter::DISCONNECTED:
		auto it = users_.begin();
		for (; it != users_.end(); ++it) {
			UserConnectionPtr tmp = *it;
			if (tmp->getId() == fromId) {
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
				player->triggerGameOverEvent();
			}
		}
		break;
	};

	// Send packet to clients.
	return true;
}

// Receives data (data) received from user with id (id).
// First element in (data) must be of a value
// defined in PacketType.
void Protocol::receiveData(const mw::Packet& data, int id) {
	PacketType type = static_cast<PacketType>(data[0]);
	switch (type) {
	case PacketType::SERVERINFO:
		{
			// Game started?
			if (start_) {
				throw ProtocolError();
			}

			// Sent from a client?
			if (id != network_->getServerId()) {
				throw ProtocolError();
			}

			// This is not a server?
			if (network_->getId() != network_->getServerId()) {
				// Only real clients (i.e. not server).
				clientReceiveServerInfo(data);
				std::cout << "\nClientReceiveServerInfo" << std::endl;
			}

			std::cout << "\nSERVERINFO" << std::endl;
		}
		break;
	case PacketType::INPUT:
		{
			// Game not started?
			if (!start_) {
				throw ProtocolError();
			}

			if (data.size() != 4) {
				throw ProtocolError();
			}

			char playerId;
			Move move;
			BlockType next;
			receivInput(data,playerId,move,next);

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
		}
		break;
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
				signalEvent(std::make_shared<GameReady>(user.getId(),user.isReady()));
				return false; // User found, stop looking!
			}
			// User not found, keep iterating!!
			return true;
		});

		std::cout << "\n" << "READY" << std::endl;
		break;
	case PacketType::TETRIS:
		{
			// Game not started?
			if (!start_) {
				throw ProtocolError();
			}

			if (data.size() < 2) {
				throw ProtocolError();
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
			std::cout << "\n" << "TETRIS" << std::endl;
		}
		break;
	case PacketType::STARTGAME:
		// Signals the game to start. Only sent by the server.

		// Is not sended by server?
		if (network_->getServerId() != id) {
			throw ProtocolError();
		}

		// This is not a server?
		if (network_->getServerId() != network_->getId()) {
			clientStartGame();
		}
		break;
	case PacketType::PAUSE:
		if (data.size() != 1) {
			throw ProtocolError();
		}
		pause_ = !pause_;

		// Signals the gui that the game begins.
		signalEvent(std::make_shared<GamePause>(pause_));
		break;
	case PacketType::STARTBLOCK:
		// Remote user?
		if (network_->getId() != id) {
			receiveStartBlock(data,id);
		}

		std::cout << "\n" << "STARTBLOCK" << std::endl;
		break;
	case PacketType::CLIENTINFO:
		// Sent by yourself to the server. Do nothing.
		break;
	default:
		throw ProtocolError();
		break;
	}
}

void Protocol::iterateAllPlayersInfo(std::function<bool(PlayerInfoPtr)> nextPlayer) const {
	iterateAllPlayers(nextPlayer); // Is this dangerous??
}

void Protocol::iterateAllPlayers(std::function<bool(PlayerPtr)> nextPlayer) const {
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

void Protocol::iterateUserConnections(std::function<bool(const UserConnection&)> nextUserConnection) const {
	for (const UserConnectionPtr user : users_) {
		bool next = nextUserConnection(*user);
		if (!next) {
			break;
		}
	}
}

void Protocol::iterateUserConnections(std::function<bool(UserConnection&)> nextUserConnection) {
	for (UserConnectionPtr user : users_) {
		bool next = nextUserConnection(*user);
		if (!next) {
			break;
		}
	}
}

bool Protocol::isAllUsersReady() {
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

void Protocol::serverReceiveClientInfo(UserConnectionPtr remote, mw::Packet packet) {
	PacketType type;
	packet >> type;
	char nbrOfPlayers;
	packet >> nbrOfPlayers;

	// Number of players to add or remove.
	remote->clear();

	for (int i = 0; i < nbrOfPlayers; ++i) {
		PlayerPtr player(new RemotePlayer(++playerId_, width_, height_));
		remote->add(player);
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
void Protocol::sendServerInfo() {
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
		newConnection->add(user.getId(),user.getNbrOfPlayers(),user.isReady());
		for (PlayerPtr player : user) {
			data << player->getId();
		}
		return true;
	});

	signalEvent(newConnection);
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);
	std::cout << "\nSendStartInfo" << std::endl;
}

void Protocol::clientReceiveServerInfo(mw::Packet data) {
	if (data.size() < 3) {
		throw ProtocolError();
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

		newConnection->add(id,nbrOfPlayers,ready);

		// This network (local)?
		if (id == network_->getId()) {
			localUser_ = UserConnectionPtr(new UserConnection(id));
			users_.push_back(localUser_);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				PlayerPtr player(new LocalPlayer(playerId, width_, height_, devices_[i]));
				localUser_->add(player);
				localUser_->setReady(ready);
			}
		} else {
			UserConnectionPtr user(new UserConnection(id));
			users_.push_back(user);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				PlayerPtr player(new RemotePlayer(playerId, width_, height_));
				user->add(player);
				user->setReady(ready);
			}
		}
	}

	signalEvent(newConnection);
}

// char type = CLIENTINFO
// char nbrOfPlayers
void Protocol::sendClientInfo() {
	mw::Packet data;
	data << PacketType::CLIENTINFO;
	data << devices_.size();
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);

	std::cout << "\nSendClientInfo" << std::endl;
}

// char type = STARTGAME
void Protocol::serverSendStartGame() {
	mw::Packet data;
	data << PacketType::STARTGAME;
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);

	std::cout << "\nServerSendStartGame!" << std::endl;
}

void Protocol::sendReady() {
	mw::Packet data;
	data << PacketType::READY;
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);

	std::cout << "\nSendReady!" << std::endl;
}

// char type = INPUT
// char playerId
// char move
// char nextBlockType, before move has taken effect
void Protocol::sendInput(char playerId, Move move, BlockType next) {
	mw::Packet data;
	data << PacketType::INPUT;
	data << playerId;
	data << move;
	data << next;
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);
}

void Protocol::receivInput(mw::Packet packet, char& playerId, Move& move, BlockType& next) {
	PacketType type;
	packet >> type;
	packet >> playerId;
	packet >> move;
	packet >> next;
}

void Protocol::sendStartBlock() {
	mw::Packet data;
	data << PacketType::STARTBLOCK;
	for (PlayerPtr player : *localUser_) {
		data << player->getCurrentBlock();
		data << player->getNextBlock();
	}
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);

	std::cout << "\nSendStartBlock" << std::endl;
}

void Protocol::receiveStartBlock(const mw::Packet& data, int id) {
	UserConnectionPtr user = nullptr;
	for (UserConnectionPtr tmp : users_) {
		if (tmp->getId() == id) {
			user = tmp;
			break;
		}
	}

	if (user == nullptr) {
		throw ProtocolError();
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
			player->update(current,next);
		} else {
			throw ProtocolError();
		}
		++it;
	}
	std::cout << "\nReceiveStartBlock" << std::endl;
}

void Protocol::addRowsToAllPlayersExcept(PlayerInfoPtr player, int nbrOfRows) {
	for (PlayerPtr local : *localUser_) {
		// Is the player to avoid?
		if (player != local) {
			std::vector<BlockType> blockTypes;
			for (int i = 0; i < nbrOfRows; ++i) {
				std::vector<BlockType> tmp = local->generateRow();
				blockTypes.insert(blockTypes.begin(),tmp.begin(),tmp.end());
			}
			sendTetrisInfo(local->getId(), blockTypes);
		}
	}
}

void Protocol::sendTetrisInfo(char playerId, const std::vector<BlockType>& blockTypes) {
	mw::Packet data;
	data << PacketType::TETRIS;
	data << playerId;
	for (BlockType type : blockTypes) {
		data << type;
	}
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);
}

void Protocol::sendPause() {
	mw::Packet data;
	data << PacketType::PAUSE;
	network_->pushToSendBuffer(data, mw::PacketType::RELIABLE);
}

void Protocol::setConnectToPort(int port) {
	connectToPort_ = port;
}

int Protocol::getConnectToPort() const {
	return connectToPort_;
}

void Protocol::setServerPort(int port) {
	serverPort_ = port;
}

int Protocol::getServerPort() const {
	return serverPort_;
}

void Protocol::setConnectToIp(std::string ip) {
	connectToIp_ = ip;
}

std::string Protocol::getConnectToIp() const {
	return connectToIp_;
}

void Protocol::clientStartGame() {
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
		case Protocol::LOCAL:
			signalEvent(std::make_shared<GameStart>(GameStart::LOCAL));
			break;
		case Protocol::CLIENT:
			signalEvent(std::make_shared<GameStart>(GameStart::CLIENT));
			break;
		case Protocol::SERVER:
			signalEvent(std::make_shared<GameStart>(GameStart::SERVER));
			break;
		case Protocol::WAITING_TO_CONNECT:
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
	std::cout << "\nSTARTGAME" << std::endl;
	initGame(width_,height_,maxLevel_, status_ == LOCAL);
	// Signals the gui that the game is not paused.
	signalEvent(std::make_shared<GamePause>(pause_));
}