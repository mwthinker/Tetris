#include "rawtetrisboard.h"
#include "square.h"
#include "block.h"

RawTetrisBoard::RawTetrisBoard(int columns, int rows, BlockType current, BlockType next) :
	gameboard_(rows * columns, BlockType::EMPTY),
	next_(next),
	columns_(columns), rows_(rows),
	isGameOver_(false),
	externalRowsAdded_(0),
	rowToBeRemoved_(-1) {

	// Uses the size of the board. I.e. rows_ and columns_.
	current_ = createBlock(current);
}

RawTetrisBoard::RawTetrisBoard(const std::vector<BlockType>& board, int columns, int rows, const Block& current, BlockType next) :
	RawTetrisBoard(columns, rows, current.getBlockType(), next) {

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

void RawTetrisBoard::update(Move move) {
	// Game over?
	if (isGameOver_ || collision(current_)) {
		if (!isGameOver_) {
			// Only called once, when the game becomes game over.
			isGameOver_ = true;
			triggerEvent(GameEvent::GAME_OVER);
		}
	} else {
		Block block = current_;
		switch (move) {
			case Move::GAME_OVER:
				// Only called once, when the game becomes game over.
				isGameOver_ = true;
				triggerEvent(GameEvent::GAME_OVER);
				break;
			case Move::LEFT:
				block.moveLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_LEFT);
				}
				break;
			case Move::RIGHT:
				block.moveRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			case Move::DOWN_GROUND:
				triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN_GROUND);
				do {
					current_ = block;
					block.moveDown();
				} while (!collision(block));
				triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN);
				break;
			case Move::DOWN:
				block.moveDown();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			case Move::ROTATE_RIGHT:
				block.rotateRight();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_ROTATE);
				}
				break;
			case Move::ROTATE_LEFT:
				block.rotateLeft();
				if (!collision(block)) {
					current_ = block;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_ROTATE);
				}
				break;
			case Move::DOWN_GRAVITY:
				block.moveDown();
				if (collision(block)) {
					// Collision detected, add squares to the gameboard.
					addBlockToBoard(current_);

					triggerEvent(GameEvent::BLOCK_COLLISION);

					// Remove any filled row on the gameboard.
					int nbr = removeFilledRows(current_);

					// Add rows due to some external event.
					std::vector<BlockType> squares = addExternalRows();
					if (squares.size() > 0) {
						externalRowsAdded_ = squares.size() / columns_;
						gameboard_.insert(gameboard_.begin(), squares.begin(), squares.end());
						triggerEvent(GameEvent::EXTERNAL_ROWS_ADDED);
					}

					// Update the user controlled block.
					current_ = createBlock(next_);
					triggerEvent(GameEvent::CURRENT_BLOCK_UPDATED);

					switch (nbr) {
						case 1:
							triggerEvent(GameEvent::ONE_ROW_REMOVED);
							break;
						case 2:
							triggerEvent(GameEvent::TWO_ROW_REMOVED);
							break;
						case 3:
							triggerEvent(GameEvent::THREE_ROW_REMOVED);
							break;
						case 4:
							triggerEvent(GameEvent::FOUR_ROW_REMOVED);
							break;
					}
				} else {
					current_ = block;
					triggerEvent(GameEvent::GRAVITY_MOVES_BLOCK);
				}
				break;
		}
	}
}

void RawTetrisBoard::updateNextBlock(BlockType nextBlock) {
	next_ = nextBlock;
	triggerEvent(GameEvent::NEXT_BLOCK_UPDATED);
}

void RawTetrisBoard::updateCurrentBlock(BlockType current) {
	current_ = createBlock(current);
	triggerEvent(GameEvent::CURRENT_BLOCK_UPDATED);
}

void RawTetrisBoard::updateRestart(BlockType current, BlockType next) {
	updateRestart(columns_, rows_, current, next);
}

void RawTetrisBoard::updateRestart(int columns, int rows, BlockType current, BlockType next) {
	next_ = next;
	rows_ = rows;
	columns_ = columns;
	current_ = createBlock(current);	
	rowToBeRemoved_ = -1;
	externalRowsAdded_ = 0;
	clearBoard();
	triggerEvent(GameEvent::RESTARTED);
}

const std::vector<BlockType>& RawTetrisBoard::getBoardVector() const {
	return gameboard_;
}

void RawTetrisBoard::addBlockToBoard(const Block& block) {
	// All squares in the block is added to the gameboard.
	for (const Square& sq : block) {
		board(sq.column_, sq.row_) = block.getBlockType();
	}
}

Block RawTetrisBoard::createBlock(BlockType blockType) const {
	return Block(blockType, columns_ / 2 - 1, rows_ - 4); // 4 rows are the starting area.
}

BlockType RawTetrisBoard::getBlockType(int column, int row) const {
	if (column < 0 || column >= columns_ || row < 0) {
		return BlockType::WALL;
	}
	if (row * columns_ + column >= (int) gameboard_.size()) {
		return BlockType::EMPTY;
	}
	return board(column, row);
}

bool RawTetrisBoard::collision(const Block& block) const {
	bool collision = false;

	for (const Square& sq : block) {
		if (getBlockType(sq.column_, sq.row_) != BlockType::EMPTY) {
			collision = true;
			break;
		}
	}

	return collision;
}

void RawTetrisBoard::clearBoard() {
	gameboard_.assign(rows_ * columns_, BlockType::EMPTY);
	isGameOver_ = false;
}

int RawTetrisBoard::removeFilledRows(const Block& block) {
	int row = block.getLowestRow();
	int nbr = 0; // Number of rows filled.
	const int nbrOfSquares = current_.getSize();
	for (int i = 0; i < nbrOfSquares; ++i) {
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

void RawTetrisBoard::moveRowsOneStepDown(int rowToRemove) {
	rowToBeRemoved_ = rowToRemove;
	triggerEvent(GameEvent::ROW_TO_BE_REMOVED);
	int indexStartOfRow = rowToRemove * columns_;
	// Erase the row.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.begin() + indexStartOfRow + columns_);
	
	// Is it necessary to replace the row?
	if ((int) gameboard_.size() < rows_ * columns_) {
		// Replace the removed row with an empty row at the top.
		gameboard_.insert(gameboard_.end(), columns_, BlockType::EMPTY);
	}
}