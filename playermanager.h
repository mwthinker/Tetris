#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include "remoteuser.h"
#include "human.h"
#include "protocol.h"

#include <SDL.h> // Gets definition: Uint32.
#include <string>
#include <vector>

#include <mw/servermanager.h>
#include <mw/clientmanager.h>
#include <mw/localmanager.h>

// Is responsible of localy and remotly player management. 
// It can act as as local, server or client game. It takes care 
// of the game rules, synchronization and progressing the game.
class PlayerManager : public Protocol {
public:
	// Explains what PlayerManager is.
	enum Status {WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT};

	// PlayerManager starts in status (WAITING_TO_CONNECT).
	PlayerManager();
	
	// Destorys and close any active connections.
	~PlayerManager();

	// Returns the number of PlayerManager:s connected to each other.
	// Returns one if this instance is not connected to any remote instances.
	int getNumberOfConnections() const;

	// Returns the number of total players from the index (connection).
	// Index (connection) goes from 0 to getNumberOfConnections()-1.
	// connection = 1 means returns the number of local players.
	int getNumberOfPlayers(int connection) const;

	// Initiates the choosen connection.
	void connect(const std::vector<HumanPtr>& humans, Status status = LOCAL);

	// Returns true if start() has been called. Returns False
	// when stop() has been called.
	bool isStarted();

	// Sets the game to ready. Will only change if the game is not started
	// and the PlayerManager is in status (WAITING_TO_CONNECT).
	void setReady(bool ready);

	// Returns if the game is set to ready.
	bool isReady() const;

	void restart();

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

	// Starts the game. Must be ready to start.
	void start();

	// Stops the game and aborts any active connection. Status is set to
	// (WAITING_TO_CONNECT).
	void stop();	
	
	// Updates all states and collect data from server/clients.
	// It progresses the game forward with (deltaTime) milliseconds. 
	// Polls all input from the added humans.
	// While in status (WAITING_TO_CONNECT) it does nothing.
	// Is is recomended to be called frequntly, otherwise any active connection
	// may be lossed (depending on implemetion).
	void update(Uint32 deltaTime);
	
	Status getStatus() const;

	// Is called when polling gameEvent from player. The player pointer 
	// is gathered from getPlayers();
	void applyRules(Player* player, GameEvent gameEvent);
	
	/*
	// Update to new human players. Will return true if the game has 
	// not started and is not set to ready.
	// Will else return false.
	// A call to start() has to been called before else it will fail 
	// and return false.
	bool updateAddNewHumans(const std::vector<HumanPtr>& humans);
	*/

	// Returns a vector of all players. Local and remotes players.
	// The players should only be used to get information and to collect
	// player events and calls to applyRules(Player*,GameEvent).
	const std::vector<Player*>& getPlayers() const;
private:

	Status status_;
};

#endif // PLAYERMANAGER_H
