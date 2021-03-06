#include "tetrisboard.h"

#include "square.h"
#include "block.h"
#include "block.h"

#include <vector>
#include <queue>
#include <random>

BlockType randomBlockType() {
	Random random;
	const int BLOCK_TYPE_MIN = 0;
	const int BLOCK_TYPE_MAX = 6;
	static_assert((int) BlockType::EMPTY > BLOCK_TYPE_MAX && 
		(int) BlockType::WALL > BLOCK_TYPE_MAX, "BlockType::EMPTY should not be generated");
	// Generate a block type.
	return static_cast<BlockType>(random.generateInt(BLOCK_TYPE_MIN, BLOCK_TYPE_MAX));
}

std::vector<BlockType> generateRow(const RawTetrisBoard& board, double squaresPerLength) {
	const unsigned int size = board.getColumns();

	Random random;
	std::vector<bool> row(size);
	for (unsigned int i = 0; i < size * squaresPerLength; ++i) {
		int index = random.generateInt(0, size - 1);
		unsigned int nbr = 0;
		while (nbr < size) {
			if (!row[(index + nbr) % size]) {
				row[(index + nbr) % size] = true;
				break;
			}
			++nbr;
		}
	}

	std::vector<BlockType> rows;
	// Fill the rows with block types.
	for (unsigned int i = 0; i < size; ++i) {
		BlockType blockType = BlockType::EMPTY;

		// Fill square?
		if (row[i]) {
			// Generate a block type.
			blockType = randomBlockType();
		}
		rows.push_back(blockType);
	}
	// Return the generated rows.
	return rows;
}

TetrisBoard::TetrisBoard(int nbrRows, int nbrColumns, BlockType current, BlockType next)
	: RawTetrisBoard(nbrRows, nbrColumns, current, next), turns_(0) {
}

TetrisBoard::TetrisBoard(const std::vector<BlockType>& board, int rows, int columns, Block current, BlockType next)
	: RawTetrisBoard(board, rows, columns, current, next), turns_(0) {
}

TetrisBoard::TetrisBoard(const TetrisBoard& board)
	: RawTetrisBoard(board) {
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	squaresToAdd_.clear();
	turns_ = 0;
	updateRestart(current, next);
}

void TetrisBoard::triggerEvent(GameEvent gameEvent) {
	listener_(gameEvent, *this);
	switch (gameEvent) {
		case GameEvent::NEXT_BLOCK_UPDATED:
			// Assumes a new turn.
			++turns_;
			break;
	}
}

void TetrisBoard::addRows(const std::vector<BlockType>& blockTypes) {
	if (!isGameOver()) {
		squaresToAdd_.insert(squaresToAdd_.end(), blockTypes.begin(), blockTypes.end());
	}
}

std::vector<BlockType> TetrisBoard::addExternalRows() {
	std::vector<BlockType> tmp = squaresToAdd_;
	squaresToAdd_.clear();
	return tmp;
}

mw::signals::Connection TetrisBoard::addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback) {
	return listener_.connect(callback);
}
