#ifndef TETRISGAME_H
#define TETRISGAME_H

#include "protocol.h"
#include "playerinfo.h"
#include "graphicboard.h"
#include "tetrisboard.h"

#include <vector>
#include <string>

// Is responsible of the all game components. It takes the sound, graphic,
// input, network and connects it with the core tetris game.
class TetrisGame : public Protocol {
public:
	TetrisGame();

	// Draws the game.
	void draw();

	// Returns the width in pixels.
    double getWidth() const;

	// Returns the width in pixels.
	double getHeight() const;

private:
	struct PlayerData {
		PlayerData(const PlayerInfoPtr& player) : player_(player) {
			reset();
		}

		void reset() {
			level_ = 1;
			points_ = 0;
			nbrOfClearedRows_ = 0;
			levelUpCounter_ = 0;
		}

		PlayerInfoPtr player_;
		GraphicBoard graphic_;
		int level_;			   // Current level.
		int points_;		   // Number of points.
		int nbrOfClearedRows_; // Number of rows the player cleared.
		std::string name_;     // The name of the player.
		int levelUpCounter_;   // Is used to determine when to level up.
	};

    void soundEffects(GameEvent gameEvent);
	void applyRules(PlayerData& playerData, GameEvent gameEvent);
	void updateGame(Uint32 deltaTime) override;
	void updatePlayer(PlayerData& playerData, Uint32 deltaTime);
	void initGame(int columns, int rows, int maxLevel, bool local) override;

	int nbrOfAlivePlayers_; // The total number of alive players.

	static const int nbrOfRowsToLevelUp = 10;

	// Fix timestep.
    Uint32 timeStep_;
	Uint32 accumulator_;

	int maxLevel_;
	bool local_;
	Uint32 countDown_;
	
	std::vector<PlayerData> players_;
};

#endif // TETRISGAME_H
