#include "remoteconnection.h"
#include "remoteplayer.h"
#include "protocol.h"
#include "tetrisparameters.h"
#include "connection.h"

#include <net/connection.h>

#include <vector>

RemoteConnection::RemoteConnection(int id, const net::ConnectionPtr& connection) :
	connection_(connection), id_(id), width_(TETRIS_WIDTH), height_(TETRIS_HEIGHT) {

}

RemoteConnection::~RemoteConnection() {
	if (connection_) {
		connection_->stop();
	}
}

int RemoteConnection::getNbrAiPlayers() const {
	int nbr = 0;
	for (auto& player : players_) {
		if (player->isAi()) {
			++nbr;
		}
	}

	return nbr;
}

void RemoteConnection::endGame() {
	for (std::shared_ptr<RemotePlayer>& remote : players_) {
		remote->endGame();
	}
}

void RemoteConnection::receive(net::Packet& packet) {
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
				player->setLastPosition(0);
				players_.push_back(player);
			}
			break;
		case PacketType::PLAYER_LEVEL:
			// Fall through!
		case PacketType::PLAYER_MOVE:
			// Fall through!
		case PacketType::PLAYER_TETRIS:
			// Fall through!
		case PacketType::PLAYER_NAME:
		{
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

void RemoteConnection::resizeBoard(int width, int height) {
	width_ = width;
	height_ = height;
}

bool RemoteConnection::isActive() const {
	return connection_->isActive();
}

net::Packet RemoteConnection::getClientInfo() const {
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
