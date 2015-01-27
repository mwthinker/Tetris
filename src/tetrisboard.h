#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include "block.h"
#include "rawtetrisboard.h"

#include <mw/signal.h>

#include <vector>
#include <queue>

// Represents a tetris board.
class TetrisBoard : public RawTetrisBoard {
public:
    TetrisBoard(int nbrRows, int nbrColumns, BlockType current, BlockType next);

	TetrisBoard(const TetrisBoard&);

	// Restarts the board. Resets all states. Current and next represents the two starting blocks.
	void restart(BlockType current, BlockType next);

	// Add rows to be added at the bottom of the board at the next change of the moving block.
	void addRows(const std::vector<BlockType>& blockTypes);

	inline int getNbrOfUpdates() const {
		return nbrOfUpdates_;
	}

	bool pollGameEvent(GameEvent& gameEvent);

	mw::signals::Connection addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback);

private:
	void triggerEvent(GameEvent gameEvent) override;

	std::vector<BlockType> addExternalRows() override;

	std::vector<BlockType> squaresToAdd_;
	std::queue<GameEvent> gameEvents_;
	mw::Signal<GameEvent, const TetrisBoard&> listener_;
	
	int nbrOfUpdates_;
};

#endif // TETRISBOARD_H
