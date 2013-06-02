#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "tetrisboard.h"
#include "player.h"
#include "localplayer.h"

#include "userconnection.h"
#include "localplayer.h"
#include "device.h"

#include <mw/signal.h>
#include <mw/packet.h>
#include <mw/network.h>

#include <functional>
#include <memory>

class NetworkEvent {
protected:
	NetworkEvent() {
	}

public:
    virtual ~NetworkEvent() {
    }
};

class NewConnection : public NetworkEvent {
public:
	enum Status {LOCAL,SERVER,CLIENT};

	NewConnection(Status status) {
		status_ = status;
	}

	void add(int id, int nbrOfPlayers, bool ready) {
		dataV.push_back(Data(id, nbrOfPlayers,ready));
	}

	void iterate(std::function<void(int id,int nbrOfPlayers, bool ready)> funcIdPort) {
		for (const Data& data : dataV) {
			funcIdPort(data.id_,data.nbr_,data.ready_);
		}
	}

	Status status_;
private:
	struct Data {
		Data(int id, int nbr, bool ready) {
			id_ = id;
			nbr_ = nbr;
			ready_ = ready;
		}

		int id_;
		int nbr_;
		bool ready_;
	};

	std::vector<Data> dataV;
};

class GameReady : public NetworkEvent {
public:
	GameReady(int id, bool ready) {
		id_ = id;
		ready_ = ready;
	}

	bool ready_;
	int id_;
};

class GameStart : public NetworkEvent {
public:
	enum Status {LOCAL,SERVER,CLIENT};

	GameStart(Status status) {
		status_ = status;
	}

	Status status_;
};

class GamePause : public NetworkEvent {
public:
	GamePause(bool pause) : pause_(pause) {
	}

	const bool pause_;
};

class GameOver : public NetworkEvent {
public:
    GameOver(int points) : points_(points) {
    }

    const int points_;
};

typedef std::shared_ptr<NetworkEvent> NetworkEventPtr;

// Is throwed when the data which is received violates the
// tetris protocol.
class ProtocolError {
};

// Value of first byte sent over network. Defines the packet content.
enum PacketType {
	PACKET_INPUT,       // Tetrisboard updates.
	PACKET_STARTGAME,   // The server starts the game. All user starts the game.
	PACKET_READY,       // The server/client is ready/unready to start.
	PACKET_SERVERINFO,  // Sent from the server. The info about players and tetrisboard conditions (e.g. length and width).
	PACKET_TETRIS,      // Data describing when player adds rows.
	PACKET_CLIENTINFO,  // A client send client info to the server.
	PACKET_STARTBLOCK,  // Sends the start current block and the next block.
	PACKET_PAUSE        // Pause/Unpause the game for all users.
};

mw::Packet& operator<<(mw::Packet& packet, const PacketType& type);
mw::Packet& operator>>(mw::Packet& packet, PacketType& type);

mw::Packet& operator<<(mw::Packet& packet, const Input& input);
mw::Packet& operator>>(mw::Packet& packet, Input& input);

mw::Packet& operator<<(mw::Packet& packet, const TetrisBoard::Move& move);
mw::Packet& operator>>(mw::Packet& packet, TetrisBoard::Move& move);

mw::Packet& operator<<(mw::Packet& packet, const BlockType& type);
mw::Packet& operator>>(mw::Packet& packet, BlockType& type);

class Protocol : public mw::ServerFilter {
public:
	enum Status {WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT};

	Protocol();
	virtual ~Protocol();

	void update(Uint32 deltaTime);

	void createLocalGame(const std::vector<DevicePtr>& devices);
	void createLocalGame(const std::vector<DevicePtr>& devices, int width, int height, int maxLevel);
	void createServerGame(const std::vector<DevicePtr>& devices, int port, int width, int height);
	void createClientGame(const std::vector<DevicePtr>& devices, int port, std::string ip);

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

	Status getStatus() const;

protected:
	void signalEvent(NetworkEventPtr nEvent);

	void connect(const std::vector<DevicePtr>& devices, Status status);

	void iterateAllPlayersInfo(std::function<bool(PlayerInfoPtr)> nextPlayer) const;

    void addRowsToAllPlayersExcept(PlayerInfoPtr player, int nbrOfRows);

	// Is called at the beginning of new round.
	virtual void initGame(int width, int height, int maxLevel, bool local) = 0;

private:
	void iterateAllPlayers(std::function<bool(PlayerPtr)> nextPlayer) const;
	void iterateUserConnections(std::function<bool(const UserConnection&)> nextUserConnection) const;
	void iterateUserConnections(std::function<bool(UserConnection&)> nextUserConnection);

	virtual void updateGame(double timeStep) = 0;

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
	void sendInput(char playerId, TetrisBoard::Move move, BlockType next);

	void receivInput(mw::Packet packet, char& playerId, TetrisBoard::Move& move, BlockType& next);

	// Pause/Unpause the game.
	void sendPause();

	// Sends info about a local player with id (playerId) who triggered a
	// tetris event (4 rows removal at once). The rows to be transformed to
	// all opponent players is saved in vector (blockTypes) which must be
	// in complete rows. With the upper left and block and row first.
	void sendTetrisInfo(int playerId, const std::vector<BlockType>& blockTypes);

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
};

#endif // PROTOCOL_H
