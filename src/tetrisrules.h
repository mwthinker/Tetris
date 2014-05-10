#ifndef TETRISRULES_H
#define TETRISRULES_H

#include "player.h"

#include <map>
#include <string>

class TetrisGame;

// Is responsible of the setting the rules for the players.
// E.g. how the players interact with each other.
class TetrisRules {
public:
	TetrisRules();

	// Is called at the beginning of every new game.
	void initGame(std::vector<PlayerPtr> players, int columns, int rows, int maxLevel, bool local);

	void applyRules(PlayerPtr player, GameEvent gameEvent, TetrisGame* game);

private:
	struct PlayerInfo {
		PlayerInfo() : points_(0), nbrClearedRows_(0), levelUpCounter_(0) {
		}

		PlayerPtr player_;
		int points_, nbrClearedRows_, levelUpCounter_;
	};

	int nbrOfAlivePlayers_; // The total number of alive players.
	int maxLevel_;
	bool local_;
	std::map<int, PlayerInfo> players_;
};

#endif // TETRISRULES_H
