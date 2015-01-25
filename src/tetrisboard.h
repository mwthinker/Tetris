#ifndef TETRISBOARD_H
#define TETRISBOARD_H

#include "block.h"
#include "rawtetrisboard.h"

#include <vector>
#include <queue>

// Represents a tetris board.
class TetrisBoard : public RawTetrisBoard {
public:
    TetrisBoard(int nbrRows, int nbrColumns, BlockType current, BlockType next);

	// Restarts the board. Resets all states. Current and next represents the two starting blocks.
	void restart(BlockType current, BlockType next);

	// Add rows to be added at the bottom of the board at the next change of the moving block.
	void addRows(const std::vector<BlockType>& blockTypes);

	inline int getNbrOfUpdates() const {
		return nbrOfUpdates_;
	}

	bool pollGameEvent(GameEvent& gameEvent);

private:
	void triggerEvent(GameEvent gameEvent) override;

	std::vector<BlockType> addExternalRows() override;

	std::vector<BlockType> squaresToAdd_;
	std::queue<GameEvent> gameEvents_;
	
	int nbrOfUpdates_;
};

#endif // TETRISBOARD_H
