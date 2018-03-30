#ifndef REMOTECONNECTION_H
#define REMOTECONNECTION_H

#include "remoteplayer.h"
#include "protocol.h"
#include "tetrisparameters.h"
#include "connection.h"

#include <net/connection.h>

#include <vector>

// Hold information about players from a remote connection.
class RemoteConnection : public Connection {
public:
	RemoteConnection(int id, const net::ConnectionPtr& connection);

	~RemoteConnection();

	// Return the id.
	int getId() const {
		return id_;
	}

	// The number of players are returned.
	int getNbrOfPlayers() const {
		return players_.size();
	}
	
	int getNbrHumanPlayers() const override {
		return players_.size() - RemoteConnection::getNbrAiPlayers();
	}

	int getNbrAiPlayers() const override;

	std::vector<std::shared_ptr<RemotePlayer>>::iterator begin() {
		return players_.begin();
	}

	std::vector<std::shared_ptr<RemotePlayer>>::iterator end() {
		return players_.end();
	}

	void endGame();

	void receive(net::Packet& packet);

	void resizeBoard(int width, int height);

	bool isActive() const;

	bool pollReceivePacket(net::Packet& packet) {
		return connection_->receive(packet);
	}

	void send(const net::Packet& packet) {
		connection_->send(packet);
	}

	net::Packet getClientInfo() const;

private:
	std::vector<std::shared_ptr<RemotePlayer>> players_;
	net::ConnectionPtr connection_;

	const int id_;
	int width_, height_;
};

#endif // REMOTECONNECTION_H
