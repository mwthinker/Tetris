#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "protocol.h"

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

private:
    void soundEffects(GameEvent gameEvent);
	void applyRules(PlayerInfoPtr player, GameEvent gameEvent);
	void updateGame(Uint32 deltaTime) override;
	void updatePlayer(PlayerInfoPtr player, Uint32 deltaTime);
	void initGame(int columns, int rows, int maxLevel, bool local) override;

	int nbrOfAlivePlayers_; // The total number of alive players.

	static const int nbrOfRowsToLevelUp = 10;

	// Fix timestep.
    Uint32 timeStep_;
	Uint32 accumulator_;

	int rows_, columns_, maxLevel_;
	bool local_;
	Uint32 countDown_;
	
	std::vector<PlayerInfoPtr> players_;
};

#endif // TETRISGAME_H
