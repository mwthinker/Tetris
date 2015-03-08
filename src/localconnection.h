#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include "localplayer.h"
#include "protocol.h"

#include <vector>

// Hold information about all local players.
class LocalConnection {
public:
	LocalConnection(PacketSender& packetSender) :
		packetSender_(packetSender),
		timeStep_(17),
		accumulator_(0),
		id_(SERVER_CONNECTION_ID) {

	}

	void addPlayer(int width, int height, const DevicePtr& device) {
		players_.push_back(std::make_shared<LocalPlayer>(players_.size(), width, height,
			randomBlockType(), randomBlockType(), device, packetSender_));
	}

	void restart() {
		for (auto& player : players_) {
			player->restart(player->getTetrisBoard().getBlockType(),
				player->getTetrisBoard().getNextBlockType());
		}
	}

	void setBoardSize(int width, int height) {
		for (auto& player : players_) {
			player->restart(player->getTetrisBoard().getBlockType(),
				player->getTetrisBoard().getNextBlockType());
		}
	}

	// Remove all players.
	void clear() {
		players_.clear();
	}

	// Return the number of players.
	int getNbrOfPlayers() const {
		return players_.size();
	}

	std::vector<std::shared_ptr<LocalPlayer>>::iterator begin() {
        return players_.begin();
    }

	std::vector<std::shared_ptr<LocalPlayer>>::iterator end() {
        return players_.end();
    }	

	void updateGame(Uint32 msDeltaTime) {
		// DeltaTime to big?
		if (msDeltaTime > 250) {
			// To avoid spiral of death.
			msDeltaTime = 250;
		}

		accumulator_ += msDeltaTime;
		while (accumulator_ >= timeStep_) {
			accumulator_ -= timeStep_;
			for (auto& player : players_) {
				player->update(timeStep_ / 1000.0);
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
			
			auto& board = player->getTetrisBoard();
			packet << board.getBlockType();
			packet << board.getNextBlockType();
		}
		return packet;
	}

	void setId(int id) {
		id_ = id;
	}

	int getSize() const {
		return players_.size();
	}

	void resizeBoard(int width, int height) {
		for (auto& player : players_) {
			player->resizeBoard(width, height);
		}
	}

private:
	std::vector<std::shared_ptr<LocalPlayer>> players_;
	PacketSender& packetSender_;
	
	int id_;

	// Fix timestep.
	const Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // LOCALCONNECTION_H
