#include "protocol.h"

#include "tetrisboard.h"
#include "player.h"
#include "remoteuser.h"

#include <mw/packet.h>
#include <mw/enet/server.h>
#include <mw/enet/client.h>
#include <mw/localnetwork.h>

#include <algorithm>

mw::Packet& operator<<(mw::Packet& packet, const PacketType& net) {
	packet << static_cast<char>(net);
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, PacketType& net) {
	char tmp;
	packet >> tmp;
	net = static_cast<PacketType>(tmp);
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const Input& input) {
	char data = input.rotate;
	data <<= 1;
	data += input.down;
	data <<= 1;
	data += input.left;
	data <<= 1;
	data += input.right;
	packet << data;
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, Input& input) {
	char data;
	packet >> data;
	char bit = 1;
	input.right = (bit & data) > 0;
	bit <<= 1;
	input.left = (bit & data) > 0;
	bit <<= 1;
	input.down = (bit & data) > 0;
	bit <<= 1;
	input.rotate = (bit & data) > 0;
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const TetrisBoard::Move& move) {
	packet << static_cast<char>(move);
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, TetrisBoard::Move& move) {
	char tmp;
	packet >> tmp;
	move = static_cast<TetrisBoard::Move>(tmp);
	return packet;
}

mw::Packet& operator<<(mw::Packet& packet, const BlockType& type) {
	packet << static_cast<char>(type);
	return packet;
}

mw::Packet& operator>>(mw::Packet& packet, BlockType& type) {
	char tmp;
	packet >> tmp;
	type = static_cast<BlockType>(tmp);
	return packet;
}

int Protocol::playerId_ = 0;

Protocol::Protocol() {
	pause_ = false;
	serverPort_ = 12008;
	connectToPort_ = 12008;
	start_ = false;
	connectToIp_ = "localhost";
	network_ = nullptr;
	ready_ = false;
	nbrOfAlivePlayers_ = 0;
	acceptNewConnections_ = false;
	status_ = WAITING_TO_CONNECT;
}

Protocol::~Protocol() {
	closeGame();
	delete network_;
}

void Protocol::createLocalGame(int nbrOfLocalPlayers) {
	//createNewHumanPlayers(nbrOfLocalPlayers);
	//connect(humanPlayers_,LOCAL);
}

void Protocol::createServerGame(int nbrOfLocalPlayers, int port) {
	//createNewHumanPlayers(nbrOfLocalPlayers);
	//setServerPort(port);
	//connect(humanPlayers_,SERVER);
}

void Protocol::createClientGame(int nbrOfLocalPlayers,int port, std::string ip) {
	if (status_ == WAITING_TO_CONNECT) {
		//createNewHumanPlayers(nbrOfLocalPlayers);
		//setConnectToIp(ip);
		//setConnectToPort(port);
		//connect(humanPlayers_,CLIENT);
	}
}

void Protocol::startGame() {
	// Game not started. // Connection must be active!
	if (network_ != 0 && !start_ && network_->getStatus() == mw::Network::ACTIVE) {
		// Is server.
		if (network_->getId() == network_->getServerId()) {
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

// Stops the game and aborts any active connection.
void Protocol::closeGame() {
	if (network_ != 0) {
		//players_.clear();
		start_ = false;
		ready_ = false;
		pause_ = false;
		status_ = WAITING_TO_CONNECT;

		// Disconnecting
		network_->stop();

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

bool Protocol::isPaused() const {
	return pause_;
}

void Protocol::pause() {
	//pause_;
	if (network_ != 0 && isStarted()) {
		sendPause();
	}
}

void Protocol::setReadyGame(bool ready) {
	if (network_ != 0 && !isStarted()) {
		ready_ = ready;
		sendReady();
	}
}

bool Protocol::isReady() const {
	return ready_;
}

bool Protocol::isStarted() const {
	return start_;
}

void Protocol::addCallback(mw::Signal<Protocol::NetworkEvent>::Callback callback) {
	eventHandler_.connect(callback);
}

void Protocol::signalEvent(Protocol::NetworkEvent nEvent) {
	eventHandler_(nEvent);
}

void Protocol::createNewHumanPlayers(int nbrOfLocalPlayers) {
	if (status_ == WAITING_TO_CONNECT) {
		humans_.clear();
		for (int i = 0; i < nbrOfLocalPlayers; ++i) {
			HumanPtr human(new Human());
			//humans_.push_back(PairHumanIndex(human));
		}
	}
}

void Protocol::update(Uint32 deltaTime) {
	if (network_ != 0) {
		network_->update();
		switch (network_->getStatus()) {
		case mw::Network::NOT_ACTIVE:
			//std::cout << "\nNOT_ACTIVE: " << std::endl;
			break;
		case mw::Network::ACTIVE:
			{
				mw::Packet data;
				int id = 0;
				while (id = network_->pullFromReceiveBuffer(data)) {
					receiveData(data, id);
				}

				if (isStarted()) {
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

void Protocol::restartGame() {
	// Manager is active and is server and game is started => restart,
	if (network_ != 0 && isStarted() && network_->getId() == network_->getServerId()) {
		serverSendStartGame();
	}
}

// Initiates the choosen connection.
void Protocol::connect(const std::vector<HumanPtr>& humans, Status status) {
	if (status_ == WAITING_TO_CONNECT) {
		humans_.clear(); // Clear old data.
		for (const HumanPtr& human : humans) {
			humans_.push_back(PairHumanIndex(human,-1));
		}
		delete network_;
		status_ = status;
		acceptNewConnections_ = true;

		switch (status) {
		case LOCAL:
			network_ = new mw::LocalNetwork(this);
			network_->start();
			std::cout << "\nLocal" << std::endl;
			// Add new player to all human players.
			for (PairHumanIndex& pair : humans_) {
				pair.second = players_.size();
				players_.push_back(new Player(++playerId_));
			}
			break;
		case SERVER:
			network_ = new mw::enet::Server(serverPort_,this);
			network_->start();
			// Add new player to all human players.
			for (PairHumanIndex& pair : humans_) {
				pair.second = players_.size();
				players_.push_back(new Player(++playerId_));
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
			// First message for server to send.
			//sendConnected();
			remoteUsers_.push_back(new RemoteUser(fromId));

			//sendServerInfo();
			std::cout << "\n" << "NEW_CONNECTION" << packet.size() <<std::endl;

			// Accept connection!
			return true;
		}

		// Refuse connection!
		return false;
	case mw::ServerFilter::PACKET:
		{
			PacketType type = static_cast<PacketType>(packet[0]);
			switch (type) {
			case PACKET_CLIENTINFO:
				{
					std::cout << "\n" << "PACKET_CLIENTINFO" << std::endl;

					// Game started?
					if (start_) {
						throw ProtocolError();
					}

					// Find the remote user with id (fromId).
					auto it = std::find_if(remoteUsers_.begin(),remoteUsers_.end(), [fromId] (RemoteUser* remote) {
						return remote->getId() == fromId;
					});

					// Find remote player.
					RemoteUser* remote = *it;

					// Failed to find?
					if (remote == 0) {
						throw ProtocolError();
					}

					serverReceiveClientInfo(remote,packet);

					sendServerInfo(); // Is sent before the data will be sent.
				}
				// Do not send to clients.
				return false;
			case PACKET_SERVERINFO:
				// Data not from server => ProtocolError
				if (network_->getServerId() != fromId) {
					throw ProtocolError();
				}

				return true;
			default:
				return true;
			}
		}
		break;
	case mw::ServerFilter::DISCONNECTED:
		/*RemoteUser* remote = 0;
		for (auto it = remoteUsers_.begin(); it != remoteUsers_.end(); ++it) {
		RemoteUser* tmp = *it;
		if (tmp->getId() == id) {
		remote = tmp;
		break;
		}
		}*/
		break;
	};

	// Send package to clients.
	return true;
}

// Receives data (data) received from user with id (id).
// First element in (data) must be of a value
// defined in PacketType.
void Protocol::receiveData(const mw::Packet& data, int id) {
	PacketType type = static_cast<PacketType>(data[0]);
	switch (type) {
	case PACKET_SERVERINFO:
		{
			// Game started => ProtocolError
			if (start_) {
				throw ProtocolError();
			}

			// Sent from client?
			if (id != network_->getServerId()) {
				throw ProtocolError();
			}

			// This is a server?
			if (network_->getId() != network_->getServerId()) {
				// Only real clients (i.e. not server).
				clientReceiveStartInfo(data);
				std::cout << "\n" << "clientReceiveStartInfo" << std::endl;
			}

			std::cout << "\n" << "PACKET_SERVERINFO" << std::endl;
		}
		break;
	case PACKET_INPUT:
		{
			// Game not started -> protocol error.
			if (!start_) {
				// Protocol error.
				throw ProtocolError();
			}

			if (data.size() != 4) {
				// Protocol error.
				throw ProtocolError();
			}

			char playerId;
			TetrisBoard::Move move;
			BlockType next;

			receivInput(data,playerId,move,next);

			for (Player* player : players_) {
				if (player->getId() == playerId) {
					player->update(move, next);
					break;
				}
			}

			//std::cout << "\n" << "PACKET_INPUT" << std::endl;
			break;
		}
	case PACKET_READY:
		// Game started?
		if (start_) {
			// Don't change anything, break!
			break;
		}

		// Find remote user and set it to ready!
		for (RemoteUser* remoteUser : remoteUsers_) {
			if (remoteUser->getId() == id) {
				remoteUser->setReady(true);
				break;
			}
		}

		std::cout << "\n" << "PACKET_READY" << std::endl;
		break;
	case PACKET_UNREADY:
		// Game started?
		if (start_) {
			throw ProtocolError();
		}

		if (data.size() != 1) {
			// Protocol error.
			throw ProtocolError();
		}

		for (RemoteUser* remoteUser : remoteUsers_) {
			if (remoteUser->getId() == id) {
				remoteUser->setReady(false);
				break;
			}
		}
		std::cout << "\n" << "PACKET_UNREADY" << std::endl;
		break;
	case PACKET_TETRIS:
		{
			// Game not started -> protocol error.
			if (!start_) {
				// Protocol error.
				throw ProtocolError();
			}

			if (data.size() < 2) {
				// Protocol error.
				throw ProtocolError();
			}

			int id = data[1];
			int index = 2;
			std::vector<BlockType> blockTypes;
			while (index < data.size()) {
				blockTypes.push_back(static_cast<BlockType>(data[index]));
				++index;
			}

			// Add rows to the player with correct id.
			for (Player* player : players_) {
				if (player->getId() == id) {
					player->update(blockTypes);
					break;
				}
			}

			std::cout << "\n" << "PACKET_TETRIS" << std::endl;

			break;
		}
	case PACKET_STARTGAME:
		// Signals the game to start. Only sent by the server.

		// Is not sended by server?
		if (network_->getServerId() != id) {
			throw ProtocolError();
		}

		// Game allready started?
		if (start_) {
			// Restart human players.
			for (auto it = humans_.begin(); it != humans_.end(); ++it) {
				Player* player = players_[it->second];
				// Restart player.
				player->restart();
			}
		}

		nbrOfAlivePlayers_ = players_.size();

		start_ = true;
		pause_ = false;
		// When server decides to start all clients must be ready.
		// If not they are set to ready. To avoid sync problem.
		ready_ = true;

		sendStartBlock();
		std::cout << "\n" << "PACKET_STARTGAME" << std::endl;

		// Signals the gui that the game begins.
		signalEvent(STARTS_GAME);

		break;
	case PACKET_PAUSE:
		if (data.size() != 1) {
			// Protocol error.
			throw ProtocolError();
		}
		pause_ = !pause_;
		break;
	case PACKET_STARTBLOCK:
		if (network_->getId() != id) {
			receiveStartBlock(data,id);
		}

		// Game not started -> protocol error.
		std::cout << "\n" << "PACKET_STARTBLOCK" << std::endl;
		break;
	case PACKET_CLIENTINFO:
		// Sent by yourself to the server. Do nothing.
		break;
	default:
		throw ProtocolError();
		break;
	}
}

void Protocol::serverReceiveClientInfo(RemoteUser* remote, mw::Packet packet) {
	PacketType type;
	packet >> type;
	char nbrOfPlayers;
	packet >> nbrOfPlayers;

	// Add the changes. I.e. add or remove player.
	const std::vector<int>& indexes = remote->getPlayerIndexes();

	// Number of players to add or remove.
	int toAdd = nbrOfPlayers - indexes.size();
	if (toAdd > 0) {
		// Add some players.
		while (toAdd > 0) {
			--toAdd;
			Player* player = new Player(++playerId_);
			player->tetrisBoard_.setDecideRandomBlockType(false);
			remote->add(players_.size());
			players_.push_back(player);
		}
	} else {
		// Remove some players.
		while (toAdd < 0) {
			++toAdd;
			int index = indexes.back();
			std::swap(players_[index],players_.back());

			delete players_.back();
			players_.pop_back();
			remote->popBackIndexes();
		}
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
	// Add new player to all human players.
	mw::Packet data;
	data.push_back(PACKET_SERVERINFO);
	data.push_back(network_->getId());
	data.push_back(humans_.size());
	for (PairHumanIndex& pair : humans_) {
		Player* player = players_[pair.second];
		data.push_back(player->getId());
	}
	for (RemoteUser* remote : remoteUsers_) {
		data.push_back(remote->getId());
		const std::vector<int>& indexes = remote->getPlayerIndexes();
		data.push_back(indexes.size());
		for (int index : indexes) {
			Player* player = players_[index];
			data.push_back(player->getId());
		}
	}
	network_->pushToSendBuffer(data);
	std::cout << "\nsendStartInfo" << std::endl;
}

void Protocol::clientReceiveStartInfo(mw::Packet data) {
	if (data.size() < 3) {
		throw ProtocolError();
	}

	for (RemoteUser* remote : remoteUsers_) {
		delete remote;
	}
	remoteUsers_.clear();
	for (Player* player : players_) {
		delete player;
	}
	players_.clear();

	int index = 0;
	while (++index < data.size()) {
		int id = data[index];
		int nbrOfPlayers = data[++index];
		if (id == network_->getId()) {
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				Player* player = new Player(playerId);
				humans_[i].second = players_.size();  // Maps human to player.
				players_.push_back(player);
			}
		} else {
			RemoteUser* user = new RemoteUser(id);
			remoteUsers_.push_back(user);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				Player* player = new Player(playerId);
				player->tetrisBoard_.setDecideRandomBlockType(false);
				// Add index.
				user->add(players_.size());  // Maps remoteuser to player.
				players_.push_back(player);
			}
		}
	}
}

// char type = CLIENTINFO
// char nbrOfPlayers
void Protocol::sendClientInfo() {
	mw::Packet data;
	data.push_back(PACKET_CLIENTINFO);
	data.push_back(humans_.size());
	std::cout << "\nsendClientInfo" << std::endl;
	network_->pushToSendBuffer(data);
}

// char type = STARTGAME
void Protocol::serverSendStartGame() {
	mw::Packet data;
	data.push_back(PACKET_STARTGAME);
	std::cout << "\nserverSendStartGame!" << std::endl;
	network_->pushToSendBuffer(data);
}

void Protocol::sendReady() {
	mw::Packet data;
	data.push_back(PACKET_READY);
	network_->pushToSendBuffer(data);
}

// char type = INPUT
// char playerId
// char move
// char nextBlockType, before move has taken effect
void Protocol::sendInput(char playerId, TetrisBoard::Move move, BlockType next) {
	mw::Packet data;
	data.push_back(PACKET_INPUT);
	data.push_back(playerId);
	data.push_back(move);
	data.push_back(next);
	network_->pushToSendBuffer(data);
}

void Protocol::receivInput(mw::Packet packet, char& playerId, TetrisBoard::Move& move, BlockType& next) {
	PacketType type;
	packet >> type;
	packet >> playerId;
	packet >> move;
	packet >> next;
}

void Protocol::sendStartBlock() {
	mw::Packet data;
	data.push_back(PACKET_STARTBLOCK);
	for (auto it = humans_.begin(); it != humans_.end(); ++it) {
		Player* player = players_[it->second];
		data.push_back(player->getId());
		data.push_back(player->tetrisBoard_.currentBlock().blockType());
		data.push_back(player->tetrisBoard_.nextBlock().blockType());
	}
	std::cout << "\n" << "sendStartBlock" << std::endl;
	network_->pushToSendBuffer(data);
};

void Protocol::receiveStartBlock(const mw::Packet& data, int id) {
	RemoteUser* user = 0;
	for (RemoteUser* tmp : remoteUsers_) {
		if (tmp->getId() == id) {
			user = tmp;
			break;
		}
	}

	if (user == 0) {
		throw ProtocolError();
	}

	const std::vector<int> indexes = user->getPlayerIndexes();

	int i = 1;
	while (i < data.size()) {
		int playerId = data[i];
		BlockType current = static_cast<BlockType>(data[i+1]);
		BlockType next = static_cast<BlockType>(data[i+2]);

		for (int index : indexes) {
			Player* player = players_[index];
			player->restart();
			player->tetrisBoard_.setNonRandomCurrentBlockType(current);
			player->tetrisBoard_.setNonRandomNextBlockType(next);
		}

		i = i + 3;
	}
	std::cout << "\n" << "receiveStartBlock" << std::endl;
};

void Protocol::sendTetrisInfo(int playerId, const std::vector<BlockType>& blockTypes) {
	Player* player = 0;
	std::vector<char> data;
	data.push_back(PACKET_TETRIS);
	data.push_back(playerId);
	data.insert(data.end(),blockTypes.begin(), blockTypes.end());
	network_->pushToSendBuffer(mw::Packet(data));
}

void Protocol::sendPause() {
	mw::Packet data;
	data.push_back(PACKET_PAUSE);
	network_->pushToSendBuffer(data);
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

int Protocol::getNumberOfPlayers(int connection) const {
	if (connection == 0) {
		return humans_.size();
	}
	return remoteUsers_[connection-1]->getNbrOfPlayers();
}
