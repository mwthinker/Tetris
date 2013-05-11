#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "device.h"
#include "protocol.h"

#include <vector>
#include <map>
#include <functional>

typedef std::shared_ptr<Device> DevicePtr;

// Is responsible of the all game components. It takes the sound, graphic,
// input, network and connects it with the core tetris game.
class TetrisGame : public Protocol {
public:
	TetrisGame();
	~TetrisGame();

	// Draws the game.
	void draw();

	// Returns the width in pixels.
    double getWidth() const;
    
	// Returns the width in pixels.
	double getHeight() const;

	void setInputDevice(const DevicePtr& device, int playerIndex);

private:
    void soundEffects(GameEvent gameEvent);	
	void applyRules(Player* player, GameEvent gameEvent);
	void updateGame(Uint32 deltaTime);
	    
	std::vector<HumanPtr> humanPlayers_;
	std::map<int,DevicePtr> devices_;
};

#endif // TETRISGAME_H
