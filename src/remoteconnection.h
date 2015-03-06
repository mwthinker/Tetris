#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include "remoteplayer.h"
#include "protocol.h"

#include <net/connection.h>

#include <vector>

// Hold information about all remote players.
class RemoteConnection {
public:
	RemoteConnection(int id) : id_(id) {
	}

	RemoteConnection(int id, const net::ConnectionPtr& connection) : connection_(connection), id_(id) {
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
			case PacketType::CONNECTION_BOARD_SIZE:
				packet >> width_ >> height_;
				for (auto player : players_) {
					packet.reset();
					player->receive(packet);
				}
				break;
			case PacketType::CONNECTION_INFO: {
				packet >> id_;
				while (packet.dataLeftToRead() > 0) {
					std::string name;
					packet >> name;
					int level;
					packet >> level;
					int points;
					packet >> points;
					BlockType moving;
					packet >> moving;
					BlockType next;
					packet >> next;

					int playerId = players_.size();
					auto player = std::make_shared<RemotePlayer>(playerId, width_, height_, moving, next);
					player->setName(name);
					player->setLevel(level);
					player->setPoints(points);
					players_.push_back(player);
				}
				break;
			}
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
			case PacketType::PLAYER_POINTS: {
				packet >> id_;
				int playerId;
				packet >> playerId;
				if (playerId >= 0 && playerId < (int) players_.size()) {
					packet.reset();
					players_[playerId]->receive(packet);
				}
				break;
			}
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

	net::Packet getClientInfo() const {
		net::Packet packet;
		packet << PacketType::CONNECTION_INFO;
		packet << id_;
		for (auto& player : players_) {
			packet << player->getName();
		}
		return packet;
	}

private:
	std::vector<std::shared_ptr<RemotePlayer>> players_;
	net::ConnectionPtr connection_;

	int id_;
	int width_, height_; // Current dimension for the tetris boards.
};

#endif // REMOTECONNECTION_H
