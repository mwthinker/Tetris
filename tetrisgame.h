#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "inputdevice.h"
#include "inputkeyboard.h"
#include "protocol.h"

#include <mw/sound.h>

#include <vector>
#include <functional>

typedef std::shared_ptr<InputDevice<PlayerEvent>> InputDevicePtr;

// Is responsible of the all game components. It takes the sound, graphic, 
// input, network and connects it with the core tetris game.
class TetrisGame : public Protocol {
public:
	// Explains what PlayerManager is.
	enum Status {WAITING_TO_CONNECT, LOCAL, SERVER, CLIENT};

	TetrisGame();
	~TetrisGame();
	
	void createLocalGame(int nbrLocalPlayers);
	void createServerGame(int nbrLocalPlayers, int port);
	void createClientGame(int nbrLocalPlayers, int port, std::string ip);
	
	void startGame();
	void restartGame();
	void closeGame();
	void setReadyGame(bool ready);
	bool isReady() const;
    
	// Moves the game forward in time by the amount of milliseconds
	// specified in parameter deltaTime.
	void update(Uint32 deltaTime);

	// Draws the game.
	void draw();

	// Returns the width in pixels.
    double getWidth() const;
    
	// Returns the width in pixels.
	double getHeight() const;

	void setInputDevice(const InputDevicePtr& inputDevice, int playerIndex);

private:
	void createNewHumanPlayers(int nbrOfLocalPlayers);
    void soundEffects(GameEvent gameEvent);

	void connect(const std::vector<HumanPtr>& humans, Status status);
	void applyRules(Player* player, GameEvent gameEvent);

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
	    
	std::vector<HumanPtr> humanPlayers_;    
	mw::Sound soundCollision_, soundTetris_, soundRowRemoved_;
	std::map<int,InputDevicePtr> inputDevices_;
	Status status_;
};

#endif // TETRISGAME_H
