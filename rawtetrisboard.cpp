/*
#include "rawtetrisboard.h"

#include "square.h"
#include "block.h"

#include <algorithm>

RawTetrisBoard::RawTetrisBoard(char nbrOfRows, char nbrOfColumns, BlockType current, BlockType next) {
	nbrOfRows_ = nbrOfRows;
	nbrOfColumns_ = nbrOfColumns;
	isGameOver_ = false;

	current_ = createBlock(current);
	next_ = createBlock(next);

	gameboard_ = std::vector<bool>( (nbrOfRows + 4) * (nbrOfColumns) , false);
}

void RawTetrisBoard::restart(BlockType current, BlockType next) {
	current_ = createBlock(current);
	next_ = createBlock(next);
	gameboard_ = std::vector<bool>( (nbrOfRows_ + 4) * nbrOfColumns_ , false);
	isGameOver_ = false;
}

// First checks if the game is over. Then perform the move.
void RawTetrisBoard::update(Move move) {
	// Check if gameover
	if (collision(current_)) {
		triggerEvent(GAME_OVER);
	}

	if (!isGameOver_) {
		Block block = current_;

		// Gravity move
		bool gravityCollision = false;

		switch (move) {
		case DOWN_GRAVITY:
			block.moveDown();
			gravityCollision = collision(block);
			if (!gravityCollision) {
				triggerEvent(GRAVITY_MOVES_BLOCK);
			}
			break;
		case LEFT:
			{
				Block tmp = block;
				tmp.moveLeft();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(PLAYER_MOVES_BLOCK_LEFT);
				}
				break;
			}
		case RIGHT:
			{
				Block tmp = block;
				tmp.moveRight();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			}
		case DOWN:
			{
				Block tmp = block;
				tmp.moveDown();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			}
		case ROTATE_LEFT:
			{
				Block tmp = block;
				tmp.rotateLeft();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(PLAYER_ROTATES_BLOCK);
				}
				break;
			}
		}

		if (gravityCollision) {
			// Collision detected, add squares to gameboard.
			addBlockToBoard(current_);
			
			// Remove any filled rows on the gameboard.
			int nbr = removeFilledRows(current_);

			// Add rows due to some external event.
			addExternalRows();

			// Updates the user controlled block.
			current_ = next_;

			// Add Game event
			triggerEvent(BLOCK_COLLISION);

			switch (nbr) {
			case 1:
				triggerEvent(ONE_ROW_REMOVED);
				break;
			case 2:
				triggerEvent(TWO_ROW_REMOVED);
				break;
			case 3:
				triggerEvent(THREE_ROW_REMOVED);
				break;
			case 4:
				triggerEvent(FOUR_ROW_REMOVED);
				break;
			}
		} else {
			// no collision, its journey can continue.
			current_ = block;
		}
	}
}

char RawTetrisBoard::getNbrOfRows() const {
	return nbrOfRows_;
}

char RawTetrisBoard::getNbrOfColumns() const {
	return nbrOfColumns_;
}

bool RawTetrisBoard::isGameOver() const {
	return isGameOver_;
}

const std::vector<bool>& RawTetrisBoard::getRawBoard() const {
	return gameboard_;
}

Block RawTetrisBoard::currentBlock() const {
	return current_;
}

Block RawTetrisBoard::nextBlock() const {
	return next_;
}

void RawTetrisBoard::addBlockToBoard(const Block& block) {
	// All squares in the block to be added to the board.
	for (const Square& sq :block) {
		gameboard_[toIndex(sq.row_, sq.column_)] = (sq.blockType_ != BlockType::EMPTY);
	}
}

Block RawTetrisBoard::createBlock(BlockType blockType) const {
	return Block(blockType, nbrOfRows_, nbrOfColumns_/2 - 1);
}

bool RawTetrisBoard::collision(const Block& block) const {
	bool collision = false;
	int nbrOfSquares = block.nbrOfSquares();

	for (int i = 0; i < nbrOfSquares; ++i) {
		Square square = block[i];
		if (square.row_ < 0) {
			collision = true;
			break;
		}
		if (square.column_ < 0 || square.column_ >= nbrOfColumns_) {
			collision = true;
			break;
		}
		if (isBlocked(block[i].row_, block[i].column_)) {
			collision = true;
			break;
		}
	}
	
	return collision;
}

int RawTetrisBoard::removeFilledRows(const Block& block) {
	int row = block.getLowestRow();

	int nbr = 0;
	const int nbrOfSquares = current_.nbrOfSquares();
	for (int i = 0; i < nbrOfSquares; ++i) {
		bool filled = true;
		if (row >= 0) {
			for (int column = 0; column < nbrOfColumns_; ++column) {
				if (!isBlocked(row, column)) {
					filled = false;
				}
			}
		} else {
			filled = false;
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
	int indexStartOfRow = rowToRemove * nbrOfColumns_;
	// Copy all rows abowe the row to be removed.
	std::vector<bool> upperBoard(gameboard_.begin() + indexStartOfRow + nbrOfColumns_, gameboard_.end());
	// Erase the row to be removed and all rows abowe.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.end());
	// Insert the rows that were abowe the row to be removed.
	gameboard_.insert(gameboard_.end(), upperBoard.begin(), upperBoard.end());
	// Add a empty row at the top in order of replacing the row that were removed.
	gameboard_.resize(gameboard_.size() + nbrOfColumns_, false);
}
*/