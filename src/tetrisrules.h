#ifndef TETRISRULES_H
#define TETRISRULES_H

#include "playerinteraction.h"
#include "rawtetrisboard.h"

#include <map>
#include <vector>

// Is responsible to inforce the game rules for the players.
// I.e. how the players interact with each other.
class TetrisRules {
public:
	TetrisRules();

	// Is called at the beginning of every new game.
	void initGame(std::vector<int> playerIds, int columns, int rows, int maxLevel, bool local);

	void applyRules(int playerId, int level, GameEvent gameEvent, PlayerInteraction& game);

private:
	struct PlayerInfo {
		PlayerInfo() : points_(0), nbrClearedRows_(0), levelUpCounter_(0) {
		}
		
		int points_, nbrClearedRows_, levelUpCounter_;
	};

	int nbrOfAlivePlayers_; // The total number of alive players.
	int maxLevel_;
	bool local_;
	std::map<int, PlayerInfo> players_;
};

#endif // TETRISRULES_H
