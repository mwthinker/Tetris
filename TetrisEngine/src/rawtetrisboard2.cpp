#include "rawtetrisboard2.h"
#include "square.h"
#include "block2.h"

RawTetrisBoard2::RawTetrisBoard2(int columns, int rows, BlockType current, BlockType next) :
	gameboard_(rows * columns, BlockType::EMPTY),
	next_(next),
	columns_(columns), rows_(rows),
	isGameOver_(false),
	externalRowsAdded_(0),
	rowToBeRemoved_(-1) {

	// Uses the size of the board. I.e. rows_ and columns_.
	current_ = createBlock(current);
}

RawTetrisBoard2::RawTetrisBoard2(const std::vector<BlockType>& board, int columns, int rows, const Block2& current, BlockType next) :
	RawTetrisBoard2(columns, rows, current.getBlockType(), next) {

	gameboard_.insert(gameboard_.begin(), board.begin(), board.end());

	int calcRows = gameboard_.size() / columns_; // Number of whole rows.
	int nbr = gameboard_.size() - calcRows * columns_; // The number of elements in the unfilled row.

	// To make all rows filled. Remove elements in the unfilled row.
	for (int i = 0; i < nbr; ++i) {
		gameboard_.pop_back();
	}

	// Remove unneeded rows. I.e. remove empty rows at the top which are outside the board.
	for (int row = calcRows - 1; row >= rows_; --row) {
		if (isRowEmpty(row)) {
			for (int column = 0; column < columns_; ++column) {
				gameboard_.pop_back();
			}
		}
	}

	if (collision(current)) {
		isGameOver_ = true;
	}
	current_ = current;
}

void RawTetrisBoard2::update(Move2 move) {
	// Game over?
	if (isGameOver_ || collision(current_)) {
		if (!isGameOver_) {
			// Only called once, when the game becomes game over.
			isGameOver_ = true;
			triggerEvent(GameEvent2::GAME_OVER);
		}
	} else {
		Block2 block = current_;
		switch (move) {
			case Move2::GAME_OVER:
				// Only called once, when the game becomes game over.
				isGameOver_ = true;
				triggerEvent(GameEvent2::GAME_OVER);
				break;
			case Move2::LEFT:
				block.moveLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_LEFT);
				}
				break;
			case Move2::RIGHT:
				block.moveRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			case Move2::DOWN_GROUND:
				triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_DOWN_GROUND);
				do {
					current_ = block;
					block.moveDown();
				} while (!collision(block));
				triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_DOWN);
				break;
			case Move2::DOWN:
				block.moveDown();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			case Move2::ROTATE_RIGHT:
				block.rotateRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_ROTATE);
				}
				break;
			case Move2::ROTATE_LEFT:
				block.rotateLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent2::PLAYER_MOVES_BLOCK_ROTATE);
				}
				break;
			case Move2::DOWN_GRAVITY:
				block.moveDown();
				if (collision(block)) {
					// Collision detected, add squares to the gameboard.
					addBlockToBoard(current_);

					triggerEvent(GameEvent2::BLOCK_COLLISION);

					// Remove any filled row on the gameboard.
					int nbr = removeFilledRows(current_);

					// Add rows due to some external event.
					std::vector<BlockType> squares = addExternalRows();
					if (squares.size() > 0) {
						externalRowsAdded_ = squares.size() / columns_;
						gameboard_.insert(gameboard_.begin(), squares.begin(), squares.end());
						triggerEvent(GameEvent2::EXTERNAL_ROWS_ADDED);
					}

					// Update the user controlled block.
					current_ = createBlock(next_);
					triggerEvent(GameEvent2::CURRENT_BLOCK_UPDATED);

					switch (nbr) {
						case 1:
							triggerEvent(GameEvent2::ONE_ROW_REMOVED);
							break;
						case 2:
							triggerEvent(GameEvent2::TWO_ROW_REMOVED);
							break;
						case 3:
							triggerEvent(GameEvent2::THREE_ROW_REMOVED);
							break;
						case 4:
							triggerEvent(GameEvent2::FOUR_ROW_REMOVED);
							break;
					}
				} else {
					current_ = block;
					triggerEvent(GameEvent2::GRAVITY_MOVES_BLOCK);
				}
				break;
		}
	}
}

void RawTetrisBoard2::updateNextBlock(BlockType nextBlock) {
	next_ = nextBlock;
	triggerEvent(GameEvent2::NEXT_BLOCK_UPDATED);
}

void RawTetrisBoard2::updateCurrentBlock(BlockType current) {
	current_ = createBlock(current);
	triggerEvent(GameEvent2::CURRENT_BLOCK_UPDATED);
}

void RawTetrisBoard2::updateRestart(BlockType current, BlockType next) {
	updateRestart(columns_, rows_, current, next);
}

void RawTetrisBoard2::updateRestart(int columns, int rows, BlockType current, BlockType next) {
	next_ = next;
	rows_ = rows;
	columns_ = columns;
	current_ = createBlock(current);	
	rowToBeRemoved_ = -1;
	externalRowsAdded_ = 0;
	clearBoard();
	triggerEvent(GameEvent2::RESTARTED);
}

const std::vector<BlockType>& RawTetrisBoard2::getBoardVector() const {
	return gameboard_;
}

void RawTetrisBoard2::addBlockToBoard(const Block2& block) {
	// All squares in the block is added to the gameboard.
	uint16_t bits = block.getBits();
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			if ((bits & 1) > 0) {
				board(x, y) = block.getBlockType();
			}
			bits <<= 1;
		}
	}
}

Block2 RawTetrisBoard2::createBlock(BlockType blockType) const {
	return Block2(blockType, 0, columns_ / 2 - 1, rows_ - 4); // 4 rows are the starting area.
}

BlockType RawTetrisBoard2::getBlockType(int column, int row) const {
	if (column < 0 || column >= columns_ || row < 0) {
		return BlockType::WALL;
	}
	if (row * columns_ + column >= (int) gameboard_.size()) {
		return BlockType::EMPTY;
	}
	return board(column, row);
}

uint16_t RawTetrisBoard2::extractBoardSegment(int lowX, int lowY) const {
	uint16_t boardSegment = 0;
	
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			if (board(x, y) != BlockType::EMPTY) {
				boardSegment += 1;
				boardSegment <<= 1;
			}
		}
	}
	return boardSegment;
}

bool RawTetrisBoard2::collision(const Block2& block) const {
	return ( block.getBits() & extractBoardSegment(block.getLowX(), block.getLowY()) ) > 0;
}

void RawTetrisBoard2::clearBoard() {
	gameboard_.assign(rows_ * columns_, BlockType::EMPTY);
	isGameOver_ = false;
}

int RawTetrisBoard2::removeFilledRows(const Block2& block) {
	int row = block.getLowY();
	int nbr = 0; // Number of rows filled.
	for (int i = 0; i < 4; ++i) {
		bool filled = false;
		if (row >= 0 && row * columns_ < (int) gameboard_.size() ) { // Check only rows inside the board.			
			filled = isRowFilled(row);
		}
		if (filled) {
			moveRowsOneStepDown(row);
			++nbr;
		} else {
			++row;
		}
	}
	return nbr;
}

void RawTetrisBoard2::moveRowsOneStepDown(int rowToRemove) {
	rowToBeRemoved_ = rowToRemove;
	triggerEvent(GameEvent2::ROW_TO_BE_REMOVED);
	int indexStartOfRow = rowToRemove * columns_;
	// Erase the row.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.begin() + indexStartOfRow + columns_);
	
	// Is it necessary to replace the row?
	if ((int) gameboard_.size() < rows_ * columns_) {
		// Replace the removed row with an empty row at the top.
		gameboard_.insert(gameboard_.end(), columns_, BlockType::EMPTY);
	}
}
