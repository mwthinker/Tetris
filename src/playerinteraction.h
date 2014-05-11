#ifndef PLAYERINTERACTION_H
#define PLAYERINTERACTION_H

// Interface defining how the tetris rules interacts with game engine.
class PlayerInteraction {
public:
	// Add the number of specified rows to the specified player.
	virtual void addRows(int playerId, int rows) = 0;
	
	// Forces the player to be game over. Will only change 
	// localy so make sure remote players gets the same behavior.
	virtual void forceGameOver(int playerId) = 0;
	
	// Call to signal that the game is over and the final points.
	// Is only relevant for single player game.
	virtual void gameIsOver(int points) = 0;

	// Set the level for the specified player. Will only change if it is a local player.
	virtual void setLevel(int playerId, int level) = 0;

protected:
	// To avoid virtual destructor.
	~PlayerInteraction() {
	}
};

#endif // PLAYERINTERACTION_H
