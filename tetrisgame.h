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
	void updateGame(double deltaTime) override;
	void updatePlayer(PlayerInfoPtr player, double deltaTime);
	void initGame(int nbrOfPlayers);

	int nbrOfAlivePlayers_; // The total number of alive players.

	// Fix timestep.
    double timeStep_;
	double accumulator_;
};

#endif // TETRISGAME_H
