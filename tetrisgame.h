#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "inputdevice.h"
#include "inputkeyboard.h"
#include "playermanager.h"

#include <mw/sound.h>

#include <vector>
#include <functional>

// Is responsible of the all game components. It takes the sound, graphic, 
// input, network and connects it with the core tetris game.
class TetrisGame {
public:
	TetrisGame();
	
	void setOnConnectionEvent(std::function<void (Protocol::ManagerEvent)> eventHandler) {
		manager_.setOnConnectionEvent(eventHandler);
	}
	
	void createLocalGame(int nbrLocalPlayers);
	void createServerGame(int nbrLocalPlayers, int port);
	void createClientGame(int nbrLocalPlayers, int port, std::string ip);
	void updatePlayersInGame(int nbrLocalPlayers) {}
	void startGame();
	void restartGame();
	void closeGame();
	void setReadyGame(bool ready);
	bool isReadyGame() const {
		return manager_.isReady();
	}

	PlayerManager::Status getStatus() const {
		return manager_.getStatus();
	}

	int getNumberOfConnections() const {
		return manager_.getNumberOfConnections();
	}

	int getNumberOfPlayers(int connection) const {		
		return manager_.getNumberOfPlayers(connection);
	}
	
	//bool updateAddNewHumans(int nbrLocalPlayers);

	bool isPaused() const {
		return manager_.isPaused();
	}

	void pause() {
		manager_.pause();
	}
		
	bool isConnected() const {
		return true;
	}

	// Updates the graphic to the current frame. The parameter deltaTime
	// specifies the amount of time from the last frame update.
    void graphicUpdate(Uint32 deltaTime);
    
	// Moves the game forward in time by the amount of milliseconds 
	// specified in parameter deltaTime.
	void physicUpdate(Uint32 deltaTime);
	
	// Handles all input from the users. Should be called whenever new inputs arrives.
    void eventUpdate(const SDL_Event& windowEvent);

	// Returns the width in pixels.
    double getWidth() const;
    
	// Returns the width in pixels.
	double getHeight() const;
private:
	void createNewHumanPlayers(int nbrOfLocalPlayers);

    void soundEffects(GameEvent gameEvent);

	typedef std::shared_ptr<InputDevice<PlayerEvent>> InputDevicePtr;
	    
	std::vector<HumanPtr> humanPlayers_;
    
	mw::Sound soundCollision_, soundTetris_, soundRowRemoved_;

	std::vector<InputDevicePtr> inputDevices_;
	PlayerManager manager_;
};

#endif // TETRISGAME_H
