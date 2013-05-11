#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "tetrisboard.h"
#include "player.h"
#include "human.h"

#include "remoteuser.h"
#include "human.h"

#include <mw/signal.h>
#include <mw/packet.h>
#include <mw/network.h>

// Is throwed when data is received which violates the 
// tetris protocol.
class ProtocolError {
};

// Value of first byte sent over network. Defines the packet content.
enum PacketType {
	PACKET_INPUT,       // Tetrisboard updates.
	PACKET_STARTGAME,   // The server starts the game. All user starts the game.
	PACKET_READY,       // The server/client is ready to start.
	PACKET_UNREADY,	    // The server/client is unready to start.
	PACKET_SERVERINFO,  // The info about players and tetrisboard conditions (e.g. length and width).
	PACKET_TETRIS,      // Data describing when player adds rows..
	PACKET_CLIENTINFO,  // Client send info to server.
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
	enum NetworkEvent {NEW_CONNECTION, STARTS_GAME, CONNECTING, CONNECTED_TO_SERVER};

	Protocol();
	~Protocol();

	void update(Uint32 deltaTime);

	void createLocalGame(int nbrLocalPlayers);
	void createServerGame(int nbrLocalPlayers, int port);
	void createClientGame(int nbrLocalPlayers, int port, std::string ip);

	void startGame();
	void closeGame();

	// Returns true when the game is paused.
	bool isPaused() const;
	
	// Pause/Unpause the game depending on the current state of
	// the game.
	void pause();

	void setReadyGame(bool ready);
	bool isReady() const;

	// Returns true if the game is started.
	bool isStarted() const;

	void restartGame();

	void addCallback(mw::Signal<Protocol::NetworkEvent>::Callback callback);

protected:
	void signalEvent(Protocol::NetworkEvent nEvent);

	void createNewHumanPlayers(int nbrOfLocalPlayers);

	void connect(const std::vector<HumanPtr>& humans, Status status);

	virtual void updateGame(Uint32 deltaTime) = 0;

	// Receives data (data) received from user with id (id).
	// First element in (data) must be of a value 
	// defined in PacketType.
	void receiveData(const mw::Packet& data, int id);

	// Server receives info from a client with id (id) about the number (nbrOfPlayers) 
	// of local players.
	void serverReceiveClientInfo(RemoteUser* remote, mw::Packet packet);

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

	int getNumberOfPlayers(int connection) const;	
		
	std::vector<Player*> players_; // All players.	
	int nbrOfAlivePlayers_; // The total number of alive players.
	typedef std::pair<HumanPtr,int> PairHumanIndex;  // All local players.
	std::vector<PairHumanIndex> humans_;  // Local humans.
private:
	// @Override ServerFilter. Is only called in server/local mode.
	// Data (data) is received from client (id). Type (type)
	// describes the type of event. The return value is the 
	// data which will be sent to all clients.
	bool sendThrough(const mw::Packet& packet, int fromId, int toId, Type type) override;

	// Server sends data of all the players in the game.
	void sendServerInfo();

	// Client received data from server. The server assignes id about all
	// players in the game.
	void clientReceiveStartInfo(mw::Packet data);

	// Sends information about the start block and preview block of all local players.	
	void sendStartBlock();

	// Receives the starting block from remote player.
	void receiveStartBlock(const mw::Packet& data, int id);

	mw::Signal<Protocol::NetworkEvent> eventHandler_;
	bool start_; // The game is started?
	bool ready_; // In game looby, ready to start?
	bool pause_; // Is game paused?

	int serverPort_; // The port on the local server.
	int connectToPort_; // The port on the remote server.
	std::string connectToIp_; // The ip on the remote server.	
	
	std::vector<RemoteUser*> remoteUsers_;  // All remote players.
	
	mw::Network* network_; // The connection manager.
	static int playerId_; // The id for the last added player.
	bool acceptNewConnections_; // Is true if more players are allowed to connect.
	Status status_;
};

#endif // PROTOCOL_H
