#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>

TetrisBoard::TetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next)
	: RawTetrisBoard(nbrOfRows, nbrOfColumns, current, next) {
	nbrOfUpdates_ = 0;
	setCurrentBlock(current);
	setNextBlock(next);
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	squaresToAdd_.clear();
	nextBlockQueue_ = std::queue<BlockType>();
	gameEvents_ = std::queue<GameEvent>();
	setCurrentBlock(current);
	setNextBlock(next);
	clearBoard();
	nbrOfUpdates_ = 0;
}

void TetrisBoard::triggerEvent(GameEvent gameEvent) {
	gameEvents_.push(gameEvent);
	switch (gameEvent) {
	case GameEvent::BLOCK_COLLISION:
		setNextBlock(nextBlockQueue_.front());
		nextBlockQueue_.pop();
		++nbrOfUpdates_;
		break;
	}
}

void TetrisBoard::add(BlockType next) {
	nextBlockQueue_.push(next);
}

bool TetrisBoard::pollGameEvent(GameEvent& gameEvent) {
	if (gameEvents_.empty()) {
		return false;
	}
	gameEvent = gameEvents_.front();
	gameEvents_.pop();
	return true;
}

void TetrisBoard::addRows(const std::vector<BlockType>& blockTypes) {
	squaresToAdd_.insert(squaresToAdd_.end(),blockTypes.begin(), blockTypes.end());
}

std::vector<BlockType> TetrisBoard::addExternalRows() {
	std::vector<BlockType> tmp = squaresToAdd_;
	squaresToAdd_.clear();
	return tmp;
}
