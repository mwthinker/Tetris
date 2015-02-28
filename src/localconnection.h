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
		accumulator_(0) {
		ready_ = false;
	}

	LocalConnection(int id) : id_(id) {
		ready_ = false;
	}

	LocalConnection(int id, const net::ConnectionPtr& connection) : connection_(connection), id_(id) {
		ready_ = false;
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
	void add(std::shared_ptr<LocalPlayer> player) {
		players_.push_back(player);
	}

	// Remove all players.
	void clear() {
		players_.clear();
	}

	// Return the number of players contain.
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

	bool receive(net::Packet& packet) {
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

private:
	std::vector<std::shared_ptr<LocalPlayer>> players_;
	net::ConnectionPtr connection_;

	const int id_; // Server/client id.
	bool ready_; // Is ready to start.

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // LOCALCONNECTION_H
