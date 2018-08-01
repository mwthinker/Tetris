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

	LocalConnection(PacketSender& packetSender);

	void setPlayers(int width, int height, const std::vector<IDevicePtr>& devices);

	void removeAllPlayers();

	void addPlayer(int width, int height, const PlayerData& playerData);

	void restart();

	void restart(int width, int height);

	// Return the number of players.
	int getNbrOfPlayers() const;

	int getNbrHumanPlayers() const override;

	int getNbrAiPlayers() const override;

	iterator begin();

	iterator end();

	const_iterator begin() const;

	const_iterator end() const;

	void updateGame(double deltaTime);

	net::Packet getClientInfo() const;

	void setId(int id);

	int getId() const;

	int getSize() const;

private:
	bool isMultiplayerGame() const;

	void sendConnectionStartBlock();

	std::vector<std::shared_ptr<LocalPlayer>> players_;
	PacketSender& packetSender_;

	int id_;

	// Fix timestep.
	static const double timeStep_;
};

inline LocalConnection::iterator LocalConnection::begin() {
	return players_.begin();
}

inline LocalConnection::iterator LocalConnection::end() {
	return players_.end();
}

inline LocalConnection::const_iterator LocalConnection::begin() const {
	return players_.begin();
}

inline LocalConnection::const_iterator LocalConnection::end() const {
	return players_.end();
}

// Return the number of players.
inline int LocalConnection::getNbrOfPlayers() const {
	return players_.size();
}

inline int LocalConnection::getNbrHumanPlayers() const {
	return players_.size() - LocalConnection::getNbrAiPlayers();
}

inline void LocalConnection::removeAllPlayers() {
	players_.clear();
}

inline int LocalConnection::getId() const {
	return id_;
}

inline int LocalConnection::getSize() const {
	return players_.size();
}

inline bool LocalConnection::isMultiplayerGame() const {
	return players_.size() > 1 && packetSender_.isActive();
}

#endif // LOCALCONNECTION_H
