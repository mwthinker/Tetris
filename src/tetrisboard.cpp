#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>

TetrisBoard::TetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next)
	: RawTetrisBoard(nbrOfRows, nbrOfColumns, current, next) {
	nbrOfUpdates_ = 0;
	setCurrentBlock(current);
	setNextBlockType(next);
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	squaresToAdd_.clear();
	setCurrentBlock(current);
	setNextBlockType(next);
	clearBoard();
	nbrOfUpdates_ = 0;
}

void TetrisBoard::triggerEvent(GameEvent gameEvent, int row1, int row2, int row3, int row4) {
	gameEvents_.push(gameEvent);
	switch (gameEvent) {
	case GameEvent::BLOCK_COLLISION:
		++nbrOfUpdates_;
		break;
	}
}

void TetrisBoard::setNextBlockType(BlockType next) {
	RawTetrisBoard::setNextBlockType(next);
}

void TetrisBoard::addRows(const std::vector<BlockType>& blockTypes) {
	squaresToAdd_.insert(squaresToAdd_.end(),blockTypes.begin(), blockTypes.end());
}

std::vector<BlockType> TetrisBoard::addExternalRows() {
	std::vector<BlockType> tmp = squaresToAdd_;
	squaresToAdd_.clear();
	return tmp;
}

bool TetrisBoard::pollGameEvent(GameEvent& gameEvent) {
	if (gameEvents_.empty()) {
		return false;
	}

	gameEvent = gameEvents_.front();
	gameEvents_.pop();
	return true;
}
