#include "protocol.h"

#include "tetrisboard.h"
#include "player.h"
#include "remoteuser.h"

#include <mw/packet.h>

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

//
int Protocol::playerId_ = 0;

Protocol::Protocol() {
	pause_ = false;
}

bool Protocol::isPaused() const {
	return pause_;
}

void Protocol::pause() {
	//pause_;
	if (manager_ != 0 && isStarted()) {
		sendPause();
	}
}

bool Protocol::isStarted() const {
	return start_;
}

void Protocol::addCallback(mw::Signal<Protocol::ManagerEvent>::Callback callback) {
	eventHandler_.connect(callback);
}

void Protocol::signalEvent(Protocol::ManagerEvent mEvent) {
	eventHandler_(mEvent);
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
				if (manager_->getServerId() != fromId) {
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
			if (id != manager_->getServerId()) {
				throw ProtocolError();
			}

			// This is a server?
			if (manager_->getId() != manager_->getServerId()) {
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
					player->tetrisBoard_.setNonRandomNextBlockType(next);
					player->update(move);
					break;
				}
			}

			//std::cout << "\n" << "PACKET_INPUT" << std::endl;
			break;
		}
	case PACKET_READY:
		// Game started?
		if (start_) {
			// Dont change anything.
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
					player->tetrisBoard_.addRows(blockTypes);
					break;
				}
			}

			std::cout << "\n" << "PACKET_TETRIS" << std::endl;

			break;
		}
	case PACKET_STARTGAME:
		// Signals the game to start. Only sent by the server.

		// Is not sended by server?
		if (manager_->getServerId() != id) {			
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
		if (manager_->getId() != id) {
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
	data.push_back(manager_->getId());
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
	manager_->pushToSendBuffer(data);
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
		if (id == manager_->getId()) {
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
	manager_->pushToSendBuffer(data);
}

// char type = STARTGAME	
void Protocol::serverSendStartGame() {
	mw::Packet data;
	data.push_back(PACKET_STARTGAME);
	std::cout << "\nserverSendStartGame!" << std::endl;
	manager_->pushToSendBuffer(data);
}

void Protocol::sendReady() {
	mw::Packet data;
	data.push_back(PACKET_READY);
	manager_->pushToSendBuffer(data);
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
	manager_->pushToSendBuffer(data);		
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
	manager_->pushToSendBuffer(data);
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
	manager_->pushToSendBuffer(mw::Packet(data));
}

void Protocol::sendPause() {
	mw::Packet data;
	data.push_back(PACKET_PAUSE);
	manager_->pushToSendBuffer(data);
}