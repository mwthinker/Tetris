#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "tetrisrules.h"
#include "player.h"
#include "networkevent.h"
#include "userconnection.h"
#include "device.h"
#include "ai.h"

#include <mw/signal.h>
#include <mw/packet.h>
#include <mw/network.h>

#include <vector>
#include <functional>
#include <memory>

class GameHandler {
public:
	virtual void initGame(const std::vector<const PlayerPtr>& players) = 0;

	virtual void eventHandler(const PlayerPtr& player, GameEvent gameEvent) = 0;

	virtual void countDown(int countDown) = 0;

protected:
	~GameHandler() {
	}
};

class TetrisGame : public mw::ServerFilter {
public:
	enum Status {WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT};

	TetrisGame(GameHandler* gameHandler);
	~TetrisGame();

	// Updates everything. Should be called each frame.
	void update(Uint32 msDeltaTime);
	
	// Uses the same settings as last call.
	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers);
	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int width, int height, int maxLevel);
	void createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, int width, int height, int maxLevel);
	void createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, std::string ip, int maxLevel);

	void startGame();
	void closeGame();

	// Returns true when the game is paused.
	bool isPaused() const;

	// Pause/Unpause the game depending on the current state of
	// the game.
	void pause();

	void changeReadyState();
	bool isReady() const;

	// Returns true if the game is started.
	bool isStarted() const;

	void restartGame();

	void addCallback(mw::Signal<NetworkEventPtr>::Callback callback);

	int getNbrOfPlayers() const;

	inline int getMaxLevel() const {
		return maxLevel_;
	}

	inline void setAis(Ai ai1, Ai ai2, Ai ai3, Ai ai4) {
		ais_[0] = ai1;
		ais_[1] = ai2;
		ais_[2] = ai3;
		ais_[3] = ai4;
	}

	void addRowsToAllPlayersExcept(PlayerInfoPtr player, int nbrOfRows);

	void signalEvent(NetworkEventPtr nEvent);

private:
	void connect(const std::vector<DevicePtr>& devices, int nbrOfComputerPlayers, Status status);

	void iterateAllPlayersInfo(std::function<bool(PlayerInfoPtr)> nextPlayer) const;

	// Is called every frame when a game is running.
	void updateGame(Uint32 msDeltaTime);

	void iterateAllPlayers(std::function<bool(PlayerPtr)> nextPlayer) const;
	void iterateUserConnections(std::function<bool(const UserConnection&)> nextUserConnection) const;
	void iterateUserConnections(std::function<bool(UserConnection&)> nextUserConnection);
	bool isAllUsersReady();

	// Receives data (data) received from user with id (id).
	// First element in (data) must be of a value
	// defined in PacketType.
	void receiveData(const mw::Packet& data, int id);

	// Server receives info from a client with id (id) about the number (nbrOfPlayers)
	// of local players.
	void serverReceiveClientInfo(UserConnectionPtr remote, mw::Packet packet);

	// Sent by client to notify the server about number of local players.
	void sendClientInfo();

	// Is sent to notify the server that the client is ready to start the game.
	// Should be sent when client is in looby.
	void sendReady();

	// Sent by server to signal that the game starts.
	void serverSendStartGame();

	// Send the current input (input) and tetrisboard move (move) from player local player
	// with id (id).
	void sendInput(char playerId, Move move, BlockType next);

	void receivInput(mw::Packet packet, char& playerId, Move& move, BlockType& next);

	// Pause/Unpause the game.
	void sendPause();

	// Sends info about a local player with id (playerId) who triggered a
	// tetris event (4 rows removal at once). The rows to be transformed to
	// all opponent players is saved in vector (blockTypes) which must be
	// in complete rows. With the upper left and block and row first.
	void sendTetrisInfo(char playerId, const std::vector<BlockType>& blockTypes);

	// Sets the port which PlayerManager should connect to if status
	// is CLIENT. I.e. calling connect(humans, Status::CLIENT).
	void setConnectToPort(int port);

	// Returns the current port to connect to if calling
	// connect(humans, Status::CLIENT).
	int getConnectToPort() const;

	// Sets the port which PlayerManager should be connected to if status
	// is SERVER. I.e. calling connect(humans, Status::SERVER).
	void setServerPort(int port);

	// Returns the current port to to which new connection connects to.
	// This accurs when a call to connect(humans, Status::SERVER) is made.
	int getServerPort() const;

	// Sets the ip to the server to connect to.
	// This ip us used when a call to connect(humans, Status::SERVER) is made.
	void setConnectToIp(std::string ip);

	// Returns the ip to the server.
	std::string getConnectToIp() const;

	// @Override ServerFilter. Is only called in server/local mode.
	// Data (data) is received from client (id). Type (type)
	// describes the type of event. The return value is the
	// data which will be sent to all clients.
	bool sendThrough(const mw::Packet& packet, int fromId, int toId, Type type) override;

	// Server sends data of all the players in the game.
	void sendServerInfo();

	// Client received data from server. The server assignes id about all
	// players in the game.
	void clientReceiveServerInfo(mw::Packet data);

	// Sends information about the start block and preview block of all local players.
	void sendStartBlock();

	// Receives the starting block from remote player.
	void receiveStartBlock(const mw::Packet& data, int id);

	void clientStartGame();

	mw::Signal<NetworkEventPtr> eventHandler_;
	bool start_; // The game is started?
	bool pause_; // Is game paused?
	int nbrOfPlayers_;

	int serverPort_; // The port on the local server.
	int connectToPort_; // The port on the remote server.
	std::string connectToIp_; // The ip on the remote server.

	std::vector<UserConnectionPtr> users_;  // All users.
    UserConnectionPtr localUser_;
    std::vector<DevicePtr> devices_;

	mw::Network* network_; // The connection manager.
	int playerId_; // The id for the last added player.
	bool acceptNewConnections_; // Is true if more players are allowed to connect.
	Status status_;
	int width_, height_, maxLevel_;
	std::array<Ai, 4> ais_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;

	TetrisRules tetrisRules_;
	GameHandler* gameHandler_;

	int countDown_;
};

#endif // TETRISGAME_H
