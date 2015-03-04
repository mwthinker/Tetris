#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include "localplayer.h"
#include "protocol.h"

#include <vector>

// Hold information about all local players.
class LocalConnection {
public:
	static const int SERVER_ID = 0;

	LocalConnection(PacketSender& packetSender) :
		packetSender_(packetSender),
		timeStep_(17),
		accumulator_(0),
		ready_(false) {

	}

	void setReady(bool ready) {
		ready_ = ready;
		net::Packet packet;
		packet << PacketType::READY;
		packet << SERVER_ID;
		packetSender_.sendToAll(packet);
	}

	bool isReady() const {
		return ready_;
	}

	// Add a player.
	void addPlayer(int width, int height, const DevicePtr& device) {
		players_.push_back(std::make_shared<LocalPlayer>(players_.size(), width, height, device, packetSender_));
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

	int getSize() const {
		return players_.size();
	}

	void restart() {
		for (auto& player : players_) {
			player->restart(randomBlockType(), randomBlockType());
		}
	}

private:
	std::vector<std::shared_ptr<LocalPlayer>> players_;
	PacketSender& packetSender_;
	
	bool ready_; // Is ready to start.

	// Fix timestep.
	const Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // LOCALCONNECTION_H
