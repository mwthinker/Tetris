#ifndef TETRISRULES_H
#define TETRISRULES_H

#include "playerinfo.h"
#include "tetrisboard.h"

#include <vector>
#include <string>

// Is responsible of the setting the rules for the players.
// E.g. how the players interact with each other.
class TetrisRules {
public:
	TetrisRules();

	// Is called at the beginning of every new game.
	void initGame(std::vector<PlayerInfoPtr> players, int columns, int rows, int maxLevel, bool local);

	void applyRules(PlayerInfoPtr player, GameEvent gameEvent);

private:
	int nbrOfAlivePlayers_; // The total number of alive players.
	int maxLevel_;
	bool local_;
	std::vector<PlayerInfoPtr> players_;
};

#endif // TETRISRULES_H
