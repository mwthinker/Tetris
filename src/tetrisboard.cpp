#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>

TetrisBoard::TetrisBoard(int nbrRows, int nbrColumns, BlockType current, BlockType next)
	: RawTetrisBoard(nbrRows, nbrColumns, current, next) {
	
	nbrOfUpdates_ = 0;
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	squaresToAdd_.clear();
	nbrOfUpdates_ = 0;
	updateRestart(current, next);
}

void TetrisBoard::triggerEvent(GameEvent gameEvent) {
	gameEvents_.push(gameEvent);
	switch (gameEvent) {
	case GameEvent::BLOCK_COLLISION:
		++nbrOfUpdates_;
		break;
	}
}

void TetrisBoard::addRows(const std::vector<BlockType>& blockTypes) {
	squaresToAdd_.insert(squaresToAdd_.end(), blockTypes.begin(), blockTypes.end());
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
