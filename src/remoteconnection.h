#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include "remoteplayer.h"

#include <net/connection.h>

#include <vector>

// Hold information about all remote players.
class RemoteConnection {
public:
	RemoteConnection() : id_(1) {
		ready_ = false;
	}

	RemoteConnection(int id) : id_(id) {
		ready_ = false;
	}

	RemoteConnection(int id, const net::ConnectionPtr& connection) : connection_(connection), id_(id) {
		ready_ = false;
	}

	~RemoteConnection() {
		if (connection_) {
			connection_->stop();
		}
	}

	// Return the id.
	int getId() const {
		return id_;
	}

	bool isReady() const {
		return ready_;
	}

	// Remove all players.
	void clear() {
		players_.clear();
	}

	// The number of players are returned.
	int getNbrOfPlayers() const {
		return players_.size();
	}

	std::vector<std::shared_ptr<RemotePlayer>>::iterator begin() {
        return players_.begin();
    }

	std::vector<std::shared_ptr<RemotePlayer>>::iterator end() {
        return players_.end();
    }

	void receive(net::Packet& packet) {
		PacketType type;
		packet >> type;
		switch (type) {
			case PacketType::CLIENTINFO: {
				int nbrOfPlayers;
				packet >> nbrOfPlayers;
				for (int i = 0; i < nbrOfPlayers; ++i) {
					std::string name;
					packet >> name;
					//addPlayer(width_, height_, name);
				}
				break;
			}
			case PacketType::READY:
				ready_ = !ready_;
				break;
			case PacketType::MOVE:
				// Fall through!
			case PacketType::TETRIS:
				// Fall through!
			case PacketType::STARTBLOCK:
				break;
		}
	}

	bool isActive() const {
		return connection_->isActive();
	}

	bool pollReceivePacket(net::Packet& packet) {
		return connection_->receive(packet);
	}

	void send(const net::Packet& packet) {
		connection_->send(packet);
	}

	void stop() {
		connection_->stop();
	}

private:
	std::vector<std::shared_ptr<RemotePlayer>> players_;
	net::ConnectionPtr connection_;

	const int id_; // Server/client id.
	bool ready_; // Is ready to start.
};

#endif // REMOTECONNECTION_H
