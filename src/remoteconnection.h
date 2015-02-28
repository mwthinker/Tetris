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

	void setReady(bool ready) {
		ready_ = ready;
	}

	bool isReady() const {
		return ready_;
	}

	// Add a player.
	void add(std::shared_ptr<RemotePlayer> player) {
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

	std::vector<std::shared_ptr<RemotePlayer>>::iterator begin() {
        return players_.begin();
    }

	std::vector<std::shared_ptr<RemotePlayer>>::iterator end() {
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

private:
	std::vector<std::shared_ptr<RemotePlayer>> players_;
	net::ConnectionPtr connection_;

	const int id_; // Server/client id.
	bool ready_; // Is ready to start.
};

#endif // REMOTECONNECTION_H
