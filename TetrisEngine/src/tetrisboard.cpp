#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>
#include <random>

BlockType randomBlockType() {
	static std::random_device rd;
	static std::default_random_engine generator(rd());
	static std::uniform_int_distribution<int> distribution(0, 6);
	static_assert((int) BlockType::EMPTY > 6 && (int) BlockType::WALL > 6, "BlockType::EMPTY should not be generated");
	// Generate a block type.
	return static_cast<BlockType>(distribution(generator));
}

std::vector<BlockType> generateRow(const RawTetrisBoard& board, double squaresPerLength) {
	const unsigned int size = board.getColumns();

	static std::random_device rd;
	static std::default_random_engine generator(rd());
	std::uniform_int_distribution<int> distribution(0, size - 1);

	std::vector<bool> row(size);
	for (unsigned int i = 0; i < size * squaresPerLength; ++i) {
		int index = distribution(generator);
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
	: RawTetrisBoard(nbrRows, nbrColumns, current, next) {
	
	nbrOfUpdates_ = 0;
}

TetrisBoard::TetrisBoard(const TetrisBoard& board)
	: RawTetrisBoard(board) {
	
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	squaresToAdd_.clear();
	nbrOfUpdates_ = 0;
	updateRestart(current, next);
}

void TetrisBoard::triggerEvent(GameEvent gameEvent) {
	listener_(gameEvent, *this);
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

mw::signals::Connection TetrisBoard::addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback) {
	return listener_.connect(callback);
}
