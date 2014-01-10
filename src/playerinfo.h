#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "tetrisboard.h"

#include <string>
#include <memory>

class PlayerInfo;
typedef std::shared_ptr<PlayerInfo> PlayerInfoPtr;

class PlayerInfo {
public:
	inline virtual ~PlayerInfo() {
	}

	// Return true when a gameEvent is polled otherwise false.
	bool pollGameEvent(GameEvent& gameEvent);

	// Returns the tetrisboard.
	const TetrisBoard& getTetrisBoard() const;

	// Trigger the player to be game over.
	// May not sync perfectly over network, and can cause
	// the tetrisboard logic to behave out of sync.
	void triggerGameOverEvent();

	// Get the current block type for the board.
	BlockType getCurrentBlock() const;

	// Get the next block type for the board.
	BlockType getNextBlock() const;

	void setLevel(int level);
	int getLevel() const;	

	void setPoints(int points);
	int getPoints() const;
	void addPoints(int points);

	void setNbrClearedRows(int rows);
	int getNbrClearedRows() const;
	void addNbrClearedRows(int rows);
	
	void setLevelUpCounter(int count);
	int getLevelUpCounter() const;
	void addLevelUpCounter(int count);

	void setName(std::string name);
	std::string getName() const;

protected:
	PlayerInfo(int width, int height);

	TetrisBoard tetrisBoard_;
	int level_, points_, nbrClearedRows_, levelUpCounter_;
	std::string name_;
};

#endif // PLAYERINFO_H
