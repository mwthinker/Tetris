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
	
	void startGame();
	void restartGame();
	void closeGame();
	void setReadyGame(bool ready);
	
	bool isReadyGame() const {
		return manager_.isReady();
	}

	bool isPaused() const {
		return manager_.isPaused();
	}

	void pause() {
		manager_.pause();
	}
    
	// Moves the game forward in time by the amount of milliseconds
	// specified in parameter deltaTime.
	void update(Uint32 deltaTime);

	// Draws the game.
	void draw();
	
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
