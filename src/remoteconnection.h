#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include "remoteplayer.h"
#include "protocol.h"
#include "tetrisparameters.h"
#include "connection.h"

#include <net/connection.h>

#include <vector>

// Hold information about players from a remote connection.
class RemoteConnection : public Connection {
public:
	RemoteConnection(int id, const net::ConnectionPtr& connection) : 
		connection_(connection), id_(id), width_(TETRIS_WIDTH), height_(TETRIS_HEIGHT) {

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

	// The number of players are returned.
	int getNbrOfPlayers() const {
		return players_.size();
	}
	
	int getNbrHumanPlayers() const override {
		return players_.size() - RemoteConnection::getNbrAiPlayers();
	}

	int getNbrAiPlayers() const override {
		int nbr = 0;
		for (auto& player : players_) {
			if (player->isAi()) {
				++nbr;
			}
		}

		return nbr;
	}

	std::vector<std::shared_ptr<RemotePlayer>>::iterator begin() {
		return players_.begin();
	}

	std::vector<std::shared_ptr<RemotePlayer>>::iterator end() {
		return players_.end();
	}

	void receive(net::Packet& packet) {
		packet.reset();
		PacketType type;
		packet >> type;
		int id;
		packet >> id; // Ignore, not important.
		switch (type) {
			case PacketType::CONNECTION_START_BLOCK:
				for (auto player : players_) {
					BlockType current;
					packet >> current;
					BlockType next;
					packet >> next;
					player->restart(current, next);
				}
				break;
			case PacketType::CONNECTION_INFO:
				players_.clear();
				while (packet.dataLeftToRead() > 0) {
					std::string name;
					packet >> name;
					int level;
					packet >> level;
					int points;
					packet >> points;
					bool ai;
					packet >> ai;
					BlockType current;
					packet >> current;
					BlockType next;
					packet >> next;
					
					auto player = std::make_shared<RemotePlayer>(players_.size(), width_, height_, ai, current, next);
					player->setName(name);
					player->setLevel(level);
					player->setPoints(points);
					player->setPoints(points);
					players_.push_back(player);
				}
				break;
			case PacketType::PLAYER_MOVE:
				// Fall through!
			case PacketType::PLAYER_TETRIS:
				// Fall through!
			case PacketType::PLAYER_NAME:
				// Fall through!
			case PacketType::PLAYER_LEVEL:
				// Fall through!
			case PacketType::PLAYER_POINTS: {
				int playerId;
				packet >> playerId;
				if (playerId >= 0 && playerId < (int) players_.size()) {
					packet.reset();
					players_[playerId]->receive(packet);
				} else {
					// Protocol error.
					throw 1;
				}
				break;
			}
		}
	}

	void resizeBoard(int width, int height) {
		width_ = width;
		height_ = height;
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

	net::Packet getClientInfo() const {
		net::Packet packet;
		packet << PacketType::CONNECTION_INFO;
		packet << id_;
		for (auto& player : players_) {
			packet << player->getName();
			packet << player->getLevel();
			packet << player->getPoints();
			packet << player->isAi();

			auto& board = player->getTetrisBoard();
			packet << board.getBlockType();
			packet << board.getNextBlockType();
		}
		return packet;
	}

private:
	std::vector<std::shared_ptr<RemotePlayer>> players_;
	net::ConnectionPtr connection_;

	const int id_;
	int width_, height_;
};

#endif // REMOTECONNECTION_H
