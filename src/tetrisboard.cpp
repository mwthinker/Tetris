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
	nextBlockQueue_ = std::queue<BlockType>();
	setCurrentBlock(current);
	setNextBlockType(next);
	clearBoard();
	nbrOfUpdates_ = 0;
}

void TetrisBoard::triggerEvent(GameEvent gameEvent) {
	switch (gameEvent) {
	case GameEvent::BLOCK_COLLISION:
		setNextBlockType(nextBlockQueue_.front());
		nextBlockQueue_.pop();
		++nbrOfUpdates_;
		break;
    default:
		// Do nothing!
        break;
	}
	gameEventFunctions_(gameEvent);
}

void TetrisBoard::addGameEventListener(const mw::Signal<GameEvent>::Callback& callback) {
	gameEventFunctions_.connect(callback);
}

void TetrisBoard::removeAllListeners() {
	gameEventFunctions_.clear();
}

void TetrisBoard::add(BlockType next) {
	nextBlockQueue_.push(next);
}

void TetrisBoard::addRows(const std::vector<BlockType>& blockTypes) {
	squaresToAdd_.insert(squaresToAdd_.end(),blockTypes.begin(), blockTypes.end());
}

std::vector<BlockType> TetrisBoard::addExternalRows() {
	std::vector<BlockType> tmp = squaresToAdd_;
	squaresToAdd_.clear();
	return tmp;
}
