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

	void applyRules();

private:
	struct PlayerData {
		PlayerData(const PlayerInfoPtr& player);

		void reset();

		PlayerInfoPtr player_;
		int level_;			   // Current level.
		int points_;		   // Number of points.
		int nbrOfClearedRows_; // Number of rows the player cleared.
		std::string name_;     // The name of the player.
		int levelUpCounter_;   // Is used to determine when to level up.
	};
	
	void applyRules(PlayerData& playerData, GameEvent gameEvent);

	int nbrOfAlivePlayers_; // The total number of alive players.
	int maxLevel_;
	bool local_;
	std::vector<PlayerData> players_;
};

#endif // TETRISRULES_H
