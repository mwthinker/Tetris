#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "player.h"
#include "localplayer.h"
#include "remoteplayer.h"
#include "protocol.h"

#include "localconnection.h"
#include "remoteconnection.h"
#include "device.h"
#include "ai.h"
#include "gamehandler.h"

#include <mw/signal.h>

#include <net/packet.h>
#include <net/network.h>
#include <net/connection.h>

#include <vector>

class TetrisGameEvent;

class TetrisGame {
public:
	enum Status {WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT};

	TetrisGame();
	~TetrisGame();

	inline void setGameHandler(GameHandler* gameHandler) {
		gameHandler_ = gameHandler;
	}

	// Updates everything. Should be called each frame.
	void update(Uint32 msDeltaTime);
	
	// Uses the same settings as last call.
	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
		int width, int height, int maxLevel);
	
	void createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
		int port, int width, int height, int maxLevel);
	
	void createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers,
		int port, std::string ip, int maxLevel);

	void startGame();
	void closeGame();

	// Returns true when the game is paused.
	bool isPaused() const;

	// Pause/Unpause the game depending on the current state of
	// the game.
	void pause();

	// Returns true if the game is started.
	bool isStarted() const;

	// Restarts the active game. If the game is not started, nothing happens.
	// Only the server can restart the game.
	void restartGame();

	void addCallback(const mw::Signal<TetrisGameEvent&>::Callback& callback);

	int getNbrOfPlayers() const;

	inline int getMaxLevel() const {
		return maxLevel_;
	}

	inline void setAis(const Ai& ai1, const Ai& ai2, const Ai& ai3, const Ai& ai4) {
		ais_[0] = ai1;
		ais_[1] = ai2;
		ais_[2] = ai3;
		ais_[3] = ai4;
	}

	inline Status getStatus() const {
		return status_;
	}

private:
	class Sender : public PacketSender {
	public:
		void sendToAll(const net::Packet& packet) const override;

		void sendToAllExcept(std::shared_ptr<RemoteConnection> remoteSendNot, const net::Packet& packet) const;

		std::shared_ptr<RemoteConnection> findRemoteConnection(int connectionId);

		std::shared_ptr<RemoteConnection> addRemoteConnection(int connectionId);

		void removeDisconnectedRemoteConnections();

		std::vector<std::shared_ptr<RemoteConnection>> remoteConnections_;
		std::shared_ptr<RemoteConnection> clientConnection_;
	};

	bool areRemoteConnectionsReady();

	void serverReceive(std::shared_ptr<RemoteConnection> client, net::Packet& packet);

	void clientReceive(net::Packet& packet);

	void receiveNetworkData();

	// Server sends data of all the players in the game.
	void sendServerInfo(net::Connection connection);

	// All client will start the game.
	void clientStartGame();

	void initGame();

	void applyRules(Player& player, GameEvent gameEvent);

	mw::Signal<TetrisGameEvent&> eventHandler_;
	bool start_; // The game is started?
	bool pause_; // Is game paused?
	int nbrOfPlayers_;

	Sender sender_;  // All users.

	LocalConnection localUser_;
	int lastConnectionId_;

	net::Network network_;
	
	Status status_;
	int width_, height_, maxLevel_;
	std::array<Ai, 4> ais_;

	GameHandler* gameHandler_;
	int nbrOfAlivePlayers_; // The total number of alive players.

	int countDown_;
};

#endif // TETRISGAME_H
