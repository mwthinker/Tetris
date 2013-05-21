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
	acceptNewConnections_ = false;
	status_ = WAITING_TO_CONNECT;
	nbrOfPlayers_ = 0;
}

Protocol::~Protocol() {
	closeGame();
	delete network_;
}

void Protocol::createLocalGame(const std::vector<DevicePtr>& devices) {
	connect(devices,LOCAL);
}

void Protocol::createServerGame(const std::vector<DevicePtr>& devices, int port) {
	setServerPort(port);
	connect(devices,SERVER);
}

void Protocol::createClientGame(const std::vector<DevicePtr>& devices, int port, std::string ip) {
	if (status_ == WAITING_TO_CONNECT) {
		setConnectToIp(ip);
		setConnectToPort(port);
		connect(devices,CLIENT);
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
			for (UserConnection* remoteUser : remoteUsers_) {
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

		for (UserConnection* remoteUser : remoteUsers_) {
			delete remoteUser;
		}

		remoteUsers_.clear();
		localUser_.clear();
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

Protocol::Status Protocol::getStatus() const {
    return status_;
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
			//std::cout << "\nNOT_ACTIVE: " << std::endl;
			break;
		case mw::Network::ACTIVE:
			{
				mw::Packet data;
				int id = 0;
				while ( (id = network_->pullFromReceiveBuffer(data)) != 0 ) {
					receiveData(data, id);
				}

				if (isStarted()) {
                    iterateAllPlayers([&](Player* player) {
                        // Will only poll moves from local players.
                        // For remote players there are no moves to pull.
                        TetrisBoard::Move move;
                        while (player->pollMove(move)) {
                            sendInput(player->getId(),move,player->getTetrisBoard().nextBlock().blockType());
                        }
                        return true;
                    });

					if (!pause_) {
						updateGame(deltaTime/1000.0);
					}
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
void Protocol::connect(const std::vector<DevicePtr>& devices, Status status) {
	if (status_ == WAITING_TO_CONNECT) {
		localUser_.clear();
		delete network_;
		network_ = nullptr;
		status_ = status;
		acceptNewConnections_ = true;
		devices_.clear();
		nbrOfPlayers_ = 0;

		switch (status) {
		case WAITING_TO_CONNECT:
            break;
		case LOCAL:
			network_ = new mw::LocalNetwork(this);
			network_->start();

            localUser_ = UserConnection(network_->getId());

			// Add new player to all local players.
			for (const DevicePtr& device : devices) {
				localUser_.add(new LocalPlayer(++playerId_,device));
			}
			break;
		case SERVER:
			network_ = new mw::enet::Server(serverPort_,this);
			network_->start();

			localUser_ = UserConnection(network_->getId());

			// Add new player to all local players.
			for (const DevicePtr& device : devices) {
				localUser_.add(new LocalPlayer(++playerId_,device));
			}
			break;
		case CLIENT:
			network_ = new mw::enet::Client(connectToPort_,connectToIp_);
			network_->start();
			sendClientInfo();
			devices_ = devices;
			break;
		};
		nbrOfPlayers_ = devices.size();
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
			remoteUsers_.push_back(new UserConnection(fromId));

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
					auto it = std::find_if(remoteUsers_.begin(),remoteUsers_.end(), [fromId] (UserConnection* remote) {
						return remote->getId() == fromId;
					});

					// Find remote player.
					UserConnection* remote = *it;

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
				clientReceiveStartInfo(data);
				std::cout << "\nClientReceiveStartInfo" << std::endl;
			}

			std::cout << "\nPACKET_SERVERINFO" << std::endl;
		}
		break;
	case PACKET_INPUT:
		{
			// Game not started?
			if (!start_) {
				throw ProtocolError();
			}

			if (data.size() != 4) {
				throw ProtocolError();
			}

			char playerId;
			TetrisBoard::Move move;
			BlockType next;
			receivInput(data,playerId,move,next);

			// Iterate until the player is found.
			iterateAllPlayers([&](Player* player) {
                bool findPlayer = false;
                if (player->getId() == playerId) {
                    player->update(move, next);
                    findPlayer = true;
                }
                return !findPlayer;
			});
			//std::cout << "\n" << "PACKET_INPUT" << std::endl;
		}
		break;
	case PACKET_READY:
		// Game started?
		if (start_) {
			// Don't change anything, break!
			break;
		}

		// Find remote user and set it to ready!
		for (UserConnection* remoteUser : remoteUsers_) {
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
			throw ProtocolError();
		}

		for (UserConnection* remoteUser : remoteUsers_) {
			if (remoteUser->getId() == id) {
				remoteUser->setReady(false);
				break;
			}
		}
		std::cout << "\n" << "PACKET_UNREADY" << std::endl;
		break;
	case PACKET_TETRIS:
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
			iterateAllPlayers([&](Player* player) {
                bool findPlayer = false;
                if (player->getId() == playerId) {
                    player->update(blockTypes);
                }
                return !findPlayer;
			});
			std::cout << "\n" << "PACKET_TETRIS" << std::endl;
		}
		break;
	case PACKET_STARTGAME:
		// Signals the game to start. Only sent by the server.

		// Is not sended by server?
		if (network_->getServerId() != id) {
			throw ProtocolError();
		}

		// Game allready started?
		if (start_) {
			// Restart local players.
			for (Player* player : localUser_) {
				// Restart player.
				player->restart();
			}
		}

		start_ = true;
		pause_ = false;
		// When server decides to start all clients must be ready.
		// If not they are set to ready. To avoid sync problem.
		ready_ = true;

		sendStartBlock();
		std::cout << "\n" << "PACKET_STARTGAME" << std::endl;

		// Signals the gui that the game begins.
		signalEvent(std::make_shared<GameStart>());

		// Signals the gui that the game is not paused.
		signalEvent(std::make_shared<GamePause>(pause_));
		break;
	case PACKET_PAUSE:
		if (data.size() != 1) {
			throw ProtocolError();
		}
		pause_ = !pause_;

		// Signals the gui that the game begins.
		signalEvent(std::make_shared<GamePause>(pause_));
		break;
	case PACKET_STARTBLOCK:
		if (network_->getId() != id) {
			receiveStartBlock(data,id);
		}

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

void Protocol::iterateAllPlayers(std::function<bool(Player*)> nextPlayer) const {
    bool next = true;
    for (Player* player : localUser_) {
        next = nextPlayer(player);
        if (!next) {
            break;
        }
    }

    if (next) {
        for (UserConnection* user : remoteUsers_) {
            for (Player* player : *user) {
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
}

void Protocol::serverReceiveClientInfo(UserConnection* remote, mw::Packet packet) {
	PacketType type;
	packet >> type;
	char nbrOfPlayers;
	packet >> nbrOfPlayers;

	// Number of players to add or remove.
    remote->clear();

    for (int i = 0; i < nbrOfPlayers; ++i) {
        Player* player = new RemotePlayer(++playerId_);
        player->tetrisBoard_.setDecideRandomBlockType(false);
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
	// Add new player to all human players.
	mw::Packet data;
	data << PACKET_SERVERINFO;
	data << network_->getId();
	data << localUser_.getNbrOfPlayers();
	for (Player* player : localUser_) {
		data << player->getId();
	}
    nbrOfPlayers_ = localUser_.getNbrOfPlayers();
	for (UserConnection* remote : remoteUsers_) {
		data << remote->getId();
		data << remote->getNbrOfPlayers();
		nbrOfPlayers_ += remote->getNbrOfPlayers();
		for (Player* player : localUser_) {
            data << player->getId();
        }
	}

	network_->pushToSendBuffer(data);
	std::cout << "\nSendStartInfo" << std::endl;
}

void Protocol::clientReceiveStartInfo(mw::Packet data) {
	if (data.size() < 3) {
		throw ProtocolError();
	}

	for (UserConnection* remote : remoteUsers_) {
		delete remote;
	}
	remoteUsers_.clear();
	nbrOfPlayers_ = 0;

	int index = 0;
	while (++index < data.size()) {
		int id = data[index];
		int nbrOfPlayers = data[++index];
		nbrOfPlayers_ += nbrOfPlayers;

		// This network (local)?
		if (id == network_->getId()) {
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				Player* player = new LocalPlayer(playerId,devices_[i]);
				localUser_.add(player);
			}
		} else {
			UserConnection* user = new UserConnection(id);
			remoteUsers_.push_back(user);
			for (int i = 0; i < nbrOfPlayers; ++i) {
				int playerId = data[++index];
				Player* player = new RemotePlayer(playerId);
				player->tetrisBoard_.setDecideRandomBlockType(false);
				// Add index.
				user->add(player);
			}
		}
	}
}

// char type = CLIENTINFO
// char nbrOfPlayers
void Protocol::sendClientInfo() {
	mw::Packet data;
	data << PACKET_CLIENTINFO;
	data << localUser_.getNbrOfPlayers();
	network_->pushToSendBuffer(data);

	std::cout << "\nSendClientInfo" << std::endl;
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
	data << PACKET_INPUT;
	data << playerId;
	data << move;
	data << next;
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
	data << PACKET_STARTBLOCK;
	for (Player* player : localUser_) {
		data << player->tetrisBoard_.currentBlock().blockType();
		data << player->tetrisBoard_.nextBlock().blockType();
	}
	network_->pushToSendBuffer(data);

	std::cout << "\nSendStartBlock" << std::endl;
};

void Protocol::receiveStartBlock(const mw::Packet& data, int id) {
	UserConnection* user = nullptr;
	for (UserConnection* tmp : remoteUsers_) {
		if (tmp->getId() == id) {
			user = tmp;
			break;
		}
	}

	if (user == nullptr) {
		throw ProtocolError();
	}

	int i = 0;
	while (i < data.size()) {
		BlockType current = static_cast<BlockType>(data[++i]);
		BlockType next = static_cast<BlockType>(data[++i]);

		for (Player* player : *user) {
			player->restart();
			player->tetrisBoard_.setNonRandomCurrentBlockType(current);
			player->tetrisBoard_.setNonRandomNextBlockType(next);
		}
	}
	std::cout << "\nReceiveStartBlock" << std::endl;
};

void Protocol::addRowsToAllPlayersExcept(Player* player, int nbrOfRows) {
    // Is local player?
    if (dynamic_cast<LocalPlayer*>(player) != nullptr) {
        for (Player* tmpPlayer : localUser_) {
            if (player != tmpPlayer) {
                std::vector<BlockType> blockTypes;
                for (int i = 0; i < nbrOfRows; ++i) {
                    std::vector<BlockType> tmp = tmpPlayer->getTetrisBoard().generateRow();
                    blockTypes.insert(blockTypes.begin(),tmp.begin(),tmp.end());
                }
                sendTetrisInfo(tmpPlayer->getId(), blockTypes);
            }
        }
    }
}

void Protocol::sendTetrisInfo(int playerId, const std::vector<BlockType>& blockTypes) {
	std::vector<char> data;
	data.push_back(PACKET_TETRIS);
	data.push_back(playerId);
	data.insert(data.end(),blockTypes.begin(), blockTypes.end());
	network_->pushToSendBuffer(mw::Packet(data));
}

void Protocol::sendPause() {
	mw::Packet data;
	data << PACKET_PAUSE;
	network_->pushToSendBuffer(data);
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

int Protocol::getNumberOfPlayers(int connection) const {
	if (connection == 0) {
		return localUser_.getNbrOfPlayers();
	}
	return remoteUsers_[connection-1]->getNbrOfPlayers();
}
