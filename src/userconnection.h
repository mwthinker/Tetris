#ifndef USERCONNECTION_H
#define USERCONNECTION_H

#include "player.h"

#include <net/connection.h>

#include <vector>

class UserConnection;
typedef std::shared_ptr<UserConnection> UserConnectionPtr;

// Hold information about all players on one computer on a network.
class UserConnection {
public:
    UserConnection() : id_(1) {
		ready_ = false;
	}

	UserConnection(int id) : id_(id) {
		ready_ = false;
	}

	UserConnection(int id, const net::ConnectionPtr& connection) : connection_(connection), id_(id) {
		ready_ = false;
	}

	~UserConnection() {
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
	void add(PlayerPtr player) {
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

    std::vector<PlayerPtr>::iterator begin() {
        return players_.begin();
    }

    std::vector<PlayerPtr>::iterator end() {
        return players_.end();
    }

    std::vector<PlayerPtr>::const_iterator begin() const {
        return players_.begin();
    }

    std::vector<PlayerPtr>::const_iterator end() const {
        return players_.end();
    }

	bool isActive() const {
		return connection_->isActive();
	}

	bool receive(net::Packet& packet) {
		connection_->receive(packet);
	}

	void send(const net::Packet& packet) {
		connection_->send(packet);
	}

	void stop() {
		connection_->stop();
	}

private:
	std::vector<PlayerPtr> players_;
	net::ConnectionPtr connection_;

	const int id_; // Server/client id.
	bool ready_; // Is ready to start.
};

#endif // USERCONNECTION_H
