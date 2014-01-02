#include "rawtetrisboard.h"
#include "square.h"
#include "block.h"

#include <vector>

RawTetrisBoard::RawTetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next) {
	nbrOfRows_ = nbrOfRows;
	nbrOfColumns_ = nbrOfColumns;
	isGameOver_ = false;

	current_ = createBlock(current);
	next_ = createBlock(next);

	gameboard_ = std::vector<BlockType>( (nbrOfRows + 4) * (nbrOfColumns) , BlockType::EMPTY);
}

// First checks if the game is over. Then perform the move.
void RawTetrisBoard::update(Move move) {
	// Game over?
	if (collision(current_)) {
		triggerGameOverEvent();
	}

	if (!isGameOver_) {
		Block block = current_;

		// Collision caused by gravity.
		bool gravityCollision = false;
		switch (move) {
		case Move::DOWN_GRAVITY:
			block.moveDown();
			gravityCollision = collision(block);
			if (!gravityCollision) {
				triggerEvent(GameEvent::GRAVITY_MOVES_BLOCK);
			}
			break;
		case Move::LEFT:
			{
				Block tmp = block;
				tmp.moveLeft();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_LEFT);
				}
			}
			break;
		case Move::RIGHT:
			{
				Block tmp = block;
				tmp.moveRight();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_RIGHT);
				}
			}
			break;
		case Move::DOWN:
			{
				Block tmp = block;
				tmp.moveDown();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN);
				}
			}
			break;
        case Move::ROTATE_RIGHT:
			{
				Block tmp = block;
				tmp.rotateRight();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
			}
			break;
		case Move::ROTATE_LEFT:
			{
				Block tmp = block;
				tmp.rotateLeft();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
			}
			break;
		}

		if (gravityCollision) {
			// Collision detected, add squares to gameboard.
			addBlockToBoard(current_);
			
			// Remove any filled rows on the gameboard.
			int nbr = removeFilledRows(current_);

			// Add rows due to some external event.
			std::vector<BlockType> squares = addExternalRows();
			gameboard_.insert(gameboard_.begin(), squares.begin(), squares.end());
			gameboard_.resize(gameboard_.size() - squares.size(), BlockType::EMPTY);

			// Updates the user controlled block.
			current_ = next_;

			// Add event.
			triggerEvent(GameEvent::BLOCK_COLLISION);

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
			// No collision, its journey can continue.
			current_ = block;
		}
	}
}

void RawTetrisBoard::triggerGameOverEvent() {
	if (!isGameOver_) {
		triggerEvent(GameEvent::GAME_OVER);
		isGameOver_ = true;
	}
}

int RawTetrisBoard::getNbrOfRows() const {
	return nbrOfRows_;
}

int RawTetrisBoard::getNbrOfColumns() const {
	return nbrOfColumns_;
}

bool RawTetrisBoard::isGameOver() const {
	return isGameOver_;
}

const std::vector<BlockType>& RawTetrisBoard::getBoardVector() const {
	return gameboard_;
}

Block RawTetrisBoard::currentBlock() const {
	return current_;
}

Block RawTetrisBoard::nextBlock() const {
	return next_;
}

void RawTetrisBoard::setNextBlock(BlockType next) {
	next_ = createBlock(next);
}

void RawTetrisBoard::setCurrentBlock(BlockType current) {
	current_ = createBlock(current);
}

void RawTetrisBoard::addBlockToBoard(const Block& block) {
	// All squares in the block to be added to the board.
	for (const Square& sq :block) {
		blockType(sq.row_, sq.column_) = sq.blockType_;
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
		if (getBlockType(block[i].row_, block[i].column_) != BlockType::EMPTY) {
			collision = true;
			break;
		}
	}
	
	return collision;
}

void RawTetrisBoard::clearBoard() {
	gameboard_ = std::vector<BlockType>( (nbrOfRows_ + 4) * (nbrOfColumns_) , BlockType::EMPTY);
	isGameOver_ = false;
}

int RawTetrisBoard::removeFilledRows(const Block& block) {
	int row = block.getLowestRow();

	int nbr = 0;
	const int nbrOfSquares = current_.nbrOfSquares();
	for (int i = 0; i < nbrOfSquares; ++i) {
		bool filled = true;
		if (row >= 0) {
			for (int column = 0; column < nbrOfColumns_; ++column) {
				if (blockType(row, column) == BlockType::EMPTY) {
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
	std::vector<BlockType> upperBoard(gameboard_.begin() + indexStartOfRow + nbrOfColumns_, gameboard_.end());
	// Erase the row to be removed and all rows abowe.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.end());
	// Insert the rows that were abowe the row to be removed.
	gameboard_.insert(gameboard_.end(), upperBoard.begin(), upperBoard.end());
	// Add a empty row at the top in order of replacing the row that were removed.
	gameboard_.resize(gameboard_.size() + nbrOfColumns_, BlockType::EMPTY);
}
