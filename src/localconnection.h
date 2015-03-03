#ifndef LOCALCONNECTION_H
#define LOCALCONNECTION_H

#include "localplayer.h"

#include <net/connection.h>

#include <vector>

// Hold information about all local players.
class LocalConnection {
public:
	LocalConnection() : id_(1),
		timeStep_(17),
		accumulator_(0),
		ready_(false),
		lastPlayerId_(0) {

	}

	LocalConnection(int id) : id_(id),
		timeStep_(17),
		accumulator_(0),
		ready_(false),
		lastPlayerId_(0) {

	}

	LocalConnection(int id, const net::ConnectionPtr& connection) :
		connection_(connection),
		id_(id),
		timeStep_(17),
		accumulator_(0),
		ready_(false),
		lastPlayerId_(0) {

	}

	~LocalConnection() {
		if (connection_) {
			connection_->stop();
		}
	}

	// Return the id.
	int getId() const {
		return id_;
	}

	void setReady(bool ready) {
		ready_ = ready;
	}

	bool isReady() const {
		return ready_;
	}

	// Add a player.
	void addPlayer(int width, int height, const DevicePtr& device) {
		players_.push_back(std::make_shared<LocalPlayer>(++lastPlayerId_, width, height, device));
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
	net::ConnectionPtr connection_;

	const int id_; // Server/client id.
	bool ready_; // Is ready to start.
	int lastPlayerId_;
	//int connectionId_;

	// Fix timestep.
	const Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // LOCALCONNECTION_H
