#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "player.h"
#include "userconnection.h"
#include "device.h"
#include "ai.h"
#include "gamehandler.h"

#include <mw/signal.h>
#include <net/packet.h>
#include <net/network.h>

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
	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers);
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

	void changeReadyState();
	bool isReady() const;

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

	Status getStatus() const {
		return status_;
	}

private:
	void connect(const std::vector<DevicePtr>& devices, int nbrOfComputerPlayers, Status status);

	// Is called every frame when a game is running.
	void updateGame(Uint32 msDeltaTime);

	void iterateAllPlayers(std::function<bool(PlayerPtr)> nextPlayer) const;
	void iterateUserConnections(std::function<bool(const UserConnection&)> nextUserConnection) const;
	void iterateUserConnections(std::function<bool(UserConnection&)> nextUserConnection);
	bool isAllUsersReady();

	// Receives data (data) received from user with id (id).
	// First element in (data) must be of a value
	// defined in PacketType.
	void receiveData(const net::Packet& data, int id);

	// Server receives info from a client with id (id) about the number (nbrOfPlayers)
	// of local players.
	void serverReceiveClientInfo(UserConnectionPtr remote, net::Packet packet);

	// Sent by client to notify the server about number of local players.
	void sendClientInfo();

	// Is sent to notify the server that the client is ready to start the game.
	// Should be sent when client is in looby.
	void sendReady();

	// Send the current move from the local player corresponding to the id provided.	
	void sendMove(char playerId, Move move, BlockType next);

	void receivInput(net::Packet packet, char& playerId, Move& move, BlockType& next);

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

	void receiveToServer(const net::Packet& packet, int clientId);

	bool connectToServer(int clientId);

	void disconnectToServer(int clientId);

	// Server sends data of all the players in the game.
	void sendServerInfo();

	// Client received data from server. The server assignes id about all
	// players in the game.
	void clientReceiveServerInfo(net::Packet data);

	// Sends information about the start block and preview block of all local players.
	void sendStartBlock();

	// Receives the starting block from remote player.
	void receiveStartBlock(const net::Packet& data, int id);

	// All client will start the game.
	void clientStartGame();

	void applyRules(Player& player, GameEvent gameEvent);

	mw::Signal<TetrisGameEvent&> eventHandler_;
	bool start_; // The game is started?
	bool pause_; // Is game paused?
	int nbrOfPlayers_;

	int serverPort_; // The port on the local server.
	int connectToPort_; // The port on the remote server.
	std::string connectToIp_; // The ip on the remote server.

	std::vector<UserConnectionPtr> users_;  // All users.
    UserConnectionPtr localUser_;
    std::vector<DevicePtr> devices_;

	net::Network network_;

	int playerId_; // The id for the last added player.
	bool acceptNewConnections_; // Is true if more players are allowed to connect.
	Status status_;
	int width_, height_, maxLevel_;
	std::array<Ai, 4> ais_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
	
	GameHandler* gameHandler_;
	int nbrOfAlivePlayers_; // The total number of alive players.

	int countDown_;
};

#endif // TETRISGAME_H
