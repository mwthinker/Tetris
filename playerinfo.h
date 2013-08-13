#ifndef PLAYERINFO_H
#define PLAYERINFO_H

#include "tetrisboard.h"

#include <memory>

class PlayerInfo;
typedef std::shared_ptr<PlayerInfo> PlayerInfoPtr;

class PlayerInfo {
public:
	PlayerInfo(int width, int height);
    virtual ~PlayerInfo();

	// Is supposed to call pushMove with the appropriate move.
	// This will indirectly update the tetrisboard.
	virtual void update(double deltaTime, int level) = 0;
	
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

protected:
	TetrisBoard tetrisBoard_;
};

#endif // PLAYERINFO_H
