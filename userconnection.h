#ifndef USERCONNECTION_H
#define USERCONNECTION_H

#include <vector>

class Player;

// Hold information about all local players on a client/server.
class UserConnection {
public:
    UserConnection() : id_(1) {
		ready_ = false;
	}

	UserConnection(int id) : id_(id) {
		ready_ = false;
	}

	// Returns the id. The id should be unique for all NetworkPlayers.
	// Is decided by the server.
	int getId() const {
		return id_;
	}

	void setReady(bool ready) {
		ready_ = ready;
	}

	bool isReady() const {
		return ready_;
	}

	void add(Player* player) {
		players_.push_back(player);
	}

	void clear() {
		players_.clear();
	}

	int getNbrOfPlayers() const {
		return players_.size();
	}

    std::vector<Player*>::iterator begin() {
        return players_.begin();
    }

    std::vector<Player*>::iterator end() {
        return players_.end();
    }

    std::vector<Player*>::const_iterator begin() const {
        return players_.begin();
    }

    std::vector<Player*>::const_iterator end() const {
        return players_.end();
    }

private:
	std::vector<Player*> players_;

	int id_; // Server/client id.
	bool ready_; // Is ready to start.
};

#endif // USERCONNECTION_H
