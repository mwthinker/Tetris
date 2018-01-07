#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include "localplayer.h"
#include "protocol.h"
#include "connection.h"

#include <vector>

// Hold information about all local players.
class LocalConnection : public Connection {
public:
	using iterator = std::vector<std::shared_ptr<LocalPlayer>>::iterator;
	using const_iterator = std::vector<std::shared_ptr<LocalPlayer>>::const_iterator;

	LocalConnection(PacketSender& packetSender) :
		packetSender_(packetSender),
		timeStep_(1.0/60),
		accumulator_(0),
		id_(UNDEFINED_CONNECTION_ID) {
	}

	void setPlayers(int width, int height, const std::vector<DevicePtr>& devices) {
		players_.clear();
		for (const auto& device : devices) {
			auto player = std::make_shared<LocalPlayer>(id_, players_.size(), width, height,
				randomBlockType(), randomBlockType(), device, packetSender_);
			players_.push_back(player);
		}
		
		if (packetSender_.isActive()) {
			packetSender_.sendToAll(getClientInfo());
		}
	}

	void removeAllPlayers() {
		players_.clear();
	}

	void addPlayer(const DevicePtr& device, int width, int height, int levelUpCounter,
		int points, int level,
		Block current, BlockType next, const std::vector<BlockType>& board) {
		
		auto player = std::make_shared<LocalPlayer>(id_, players_.size(), width, height,
			board, levelUpCounter, points, level,
			current, next, device, packetSender_);
		players_.push_back(player);
	}

	void restart() {
		for (auto& player : players_) {
			player->restart(randomBlockType(),
				randomBlockType());
		}

		if (packetSender_.isActive()) {
			sendConnectionStartBlock();
		}
	}

	void resizeBoard(int width, int height) {
		for (auto& player : players_) {
			player->resizeBoard(width, height);
		}

		if (packetSender_.isActive()) {
			sendConnectionStartBlock();
		}
	}

	// Return the number of players.
	int getNbrOfPlayers() const {
		return players_.size();
	}

	int getNbrHumanPlayers() const override {
		return players_.size() - LocalConnection::getNbrAiPlayers();
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

	iterator begin() {
        return players_.begin();
    }

	iterator end() {
        return players_.end();
    }

	const_iterator begin() const {
		return players_.begin();
	}

	const_iterator end() const {
		return players_.end();
	}

	void updateGame(double deltaTime) {
		// DeltaTime to big?
		if (deltaTime > 0.250) {
			// To avoid spiral of death.
			deltaTime = 0.250;
		}

		accumulator_ += deltaTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;
			for (auto& player : players_) {
				player->update(timeStep_);
			}
		}
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

	void setId(int id) {
		id_ = id;
		for (auto& player : players_) {
			player->setConnectionId(id_);
		}
	}

	int getId() const {
		return id_;
	}

	int getSize() const {
		return players_.size();
	}

private:
	bool isMultiplayerGame() const {
		return players_.size() > 1 && packetSender_.isActive();
	}	

	void sendConnectionStartBlock() {
		net::Packet packet;
		packet << PacketType::CONNECTION_START_BLOCK;
		packet << id_;
		for (auto& player : players_) {
			packet << player->getTetrisBoard().getBlockType();
			packet << player->getTetrisBoard().getNextBlockType();
		}
		packetSender_.sendToAll(packet);
	}

	std::vector<std::shared_ptr<LocalPlayer>> players_;
	PacketSender& packetSender_;
	
	int id_;

	// Fix timestep.
	const double timeStep_;
	double accumulator_;
};

#endif // LOCALCONNECTION_H
