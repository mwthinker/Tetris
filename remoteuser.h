#ifndef REMOTEUSER_H
#define REMOTEUSER_H

#include <vector>

// Hold information about all local players on a client/server.
class RemoteUser {
public:
	RemoteUser(int id) {
		id_ = id;
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

	void add(int index) {
		playerIndexes_.push_back(index);
	}

	void popBackIndexes() {
		playerIndexes_.pop_back();
	}

	void clearIndexes() {
		playerIndexes_.clear();
	}

	int getNbrOfPlayers() const {
		return playerIndexes_.size();
	}

	const std::vector<int> getPlayerIndexes() const {
		return playerIndexes_;
	}
private:
	std::vector<int> playerIndexes_; // Player indexes_;

	int id_; // Server/client id.
	bool ready_; // Is ready to start.
};

#endif // REMOTEUSER_H
