#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "protocol.h"
#include "localconnection.h"
#include "remoteconnection.h"
#include "device.h"
#include "playerdata.h"

#include <mw/signal.h>

#include <net/packet.h>
#include <net/network.h>
#include <net/connection.h>

#include <vector>

class TetrisGameEvent;
class Player;

class TetrisGame {
public:
	enum Status { WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT };

	TetrisGame();
	~TetrisGame();

	// Updates everything. Should be called each frame.
	void update(double deltaTime);

	// Uses the same settings as last call.
	void createLocalGame();
	void createLocalGame(int rows, int columns);

	void createServerGame(int port);

	void createClientGame(int port, std::string ip);

	void resumeGame(int rows, int columns, const std::vector<PlayerData>& playersData);

	void closeGame();

	bool isPaused() const;

	// Pause/Unpause the game depending on the current state of
	// the game.
	void pause();

	// Restart the active game. If the game was not started, nothing happens.
	void restartGame();

	void addCallback(const mw::Signal<TetrisGameEvent&>::Callback& callback);

	int getNbrOfPlayers() const;

	int getMaxLevel() const {
		return maxLevel_;
	}

	void resizeBoard(int width, int height);

	int getRows() const {
		return height_;
	}

	int getColumns() const {
		return width_;
	}

	void setPlayers(const std::vector<DevicePtr>& devices);

	Status getStatus() const {
		return status_;
	}

	mw::signals::Connection addGameEventHandler(const std::function<void(TetrisGameEvent&)>& handler) {
		return eventHandler_.connect(handler);
	}

	std::vector<PlayerData> getPlayerData() const;
	
	bool currentGameHasCountDown() const {
		return nbrOfPlayers_ > 1 && countDownTime_ > 0;
	}

	bool isCurrentGameActive() const {
		return nbrOfAlivePlayers_ > 0;
	}

private:
	class Sender : public PacketSender {
	public:
		bool isActive() const override;

		void sendToAll(const net::Packet& packet) const override;

		void sendToAllExcept(std::shared_ptr<RemoteConnection> remoteSendNot, const net::Packet& packet) const;

		std::shared_ptr<RemoteConnection> findRemoteConnection(int connectionId);

		std::shared_ptr<RemoteConnection> addRemoteConnection(int connectionId, net::ConnectionPtr connection);

		void serverRemoveDisconnectedConnections();

		void removeConnection(int connectionId);

		void disconnect();

		std::vector<std::shared_ptr<RemoteConnection>>::iterator begin() {
			return remoteConnections_.begin();
		}

		std::vector<std::shared_ptr<RemoteConnection>>::iterator end() {
			return remoteConnections_.end();
		}

		void setServerConnection(const net::ConnectionPtr& connectionToServer) {
			connectionToServer_ = connectionToServer;
		}

		bool receivePacketFromServer(net::Packet& packet) {
			return connectionToServer_->receive(packet);
		}

	private:
		std::vector<std::shared_ptr<RemoteConnection>> remoteConnections_;
		net::ConnectionPtr connectionToServer_;
	};

	void serverReceive(std::shared_ptr<RemoteConnection> client, net::Packet& packet);

	void clientReceive(net::Packet& packet);

	void remoteReceive(std::shared_ptr<RemoteConnection> remoteConnection, net::Packet& packet);

	void receiveAndSendNetworkData();

	void initGame();

	void applyRulesForLocalPlayers(GameEvent gameEvent, const TetrisBoard& board, std::shared_ptr<LocalPlayer>& player);

	void applyRulesForRemotePlayers(GameEvent gameEvent, const TetrisBoard& board, std::shared_ptr<RemotePlayer>& player);

	mw::Signal<TetrisGameEvent&> eventHandler_;
	bool pause_;
	int nbrOfPlayers_;

	Sender sender_;

	LocalConnection localConnection_;
	int lastConnectionId_;

	net::Network network_;

	Status status_;
	int width_, height_, maxLevel_;

	int nbrOfAlivePlayers_;

	int countDownTime_;   // Controlling how long the start game count down should be in seconds.
	double timeLeftToStart_; // Time left for the count down.
	int wholeTimeLeft_; // Time left in whole seconds. E.g. timeLeftToStart_ = 1.4s means that wholeTimeLeft_ = 2s;
};

#endif // TETRISGAME_H
