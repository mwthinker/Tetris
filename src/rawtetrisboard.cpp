#include "rawtetrisboard.h"
#include "square.h"
#include "block.h"

#include <vector>

RawTetrisBoard::RawTetrisBoard(int nbrRows, int nbrColumns,
	BlockType current, BlockType next) :
	gameboard_((nbrRows + 4) * (nbrColumns), BlockType::EMPTY),
	next_(next),
	nbrRows_(nbrRows), nbrColumns_(nbrColumns),
	isGameOver_(false) {
	
	current_ = createBlock(current);
}

void RawTetrisBoard::update(Move move) {
	// Game over?
	if (isGameOver_ || collision(current_)) {
		if (!isGameOver_) {
			// Only called once when the game becomes game over.
			isGameOver_ = true;
			triggerEvent(GameEvent::GAME_OVER);
		}
	} else {
		Block block = current_;

		// Collision caused by gravity.
		bool gravityCollision = false;
		switch (move) {
			case Move::GAME_OVER:
				// Only called once when the game becomes game over.
				isGameOver_ = true;
				triggerEvent(GameEvent::GAME_OVER);
				break;
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
				break;
			}
			case Move::RIGHT:
			{
				Block tmp = block;
				tmp.moveRight();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			}
			case Move::DOWN:
			{
				Block tmp = block;
				tmp.moveDown();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			}
			case Move::ROTATE_RIGHT:
			{
				Block tmp = block;
				tmp.rotateRight();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
				break;
			}
			case Move::ROTATE_LEFT:
			{
				Block tmp = block;
				tmp.rotateLeft();
				if (!collision(tmp)) {
					block = tmp;
					triggerEvent(GameEvent::PLAYER_ROTATES_BLOCK);
				}
				break;
			}
		}

		if (gravityCollision) {
			// Add event.
			triggerEvent(GameEvent::BLOCK_COLLISION);

			// Collision detected, add squares to the gameboard.
			addBlockToBoard(current_);

			// Remove any filled row on the gameboard.
			int nbr = removeFilledRows(current_);

			// Add rows due to some external event.
			std::vector<BlockType> squares = addExternalRows();
			gameboard_.insert(gameboard_.begin(), squares.begin(), squares.end());
			gameboard_.resize(gameboard_.size() - squares.size(), BlockType::EMPTY);

			// Update the user controlled block.
			current_ = createBlock(next_);

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

const std::vector<BlockType>& RawTetrisBoard::getBoardVector() const {
	return gameboard_;
}

void RawTetrisBoard::setCurrentBlock(BlockType current) {
	current_ = createBlock(current);
}

void RawTetrisBoard::addBlockToBoard(const Block& block) {
	// All squares in the block is added to the gameboard.
	for (const Square& sq : block) {
		blockType(sq.row_, sq.column_) = sq.blockType_;
	}
}

Block RawTetrisBoard::createBlock(BlockType blockType) const {
	return Block(blockType, nbrRows_, nbrColumns_ / 2 - 1);
}

bool RawTetrisBoard::collision(const Block& block) const {
	bool collision = false;

	for (const Square& sq : block) {
		if (getBlockType(sq.row_, sq.column_) != BlockType::EMPTY) {
			collision = true;
			break;
		}
	}

	return collision;
}

void RawTetrisBoard::clearBoard() {
	gameboard_ = std::vector<BlockType>((nbrRows_ + 4) * (nbrColumns_), BlockType::EMPTY);
	isGameOver_ = false;
}

int RawTetrisBoard::removeFilledRows(const Block& block) {
	int row = block.getLowestRow();

	int nbr = 0;
	const int nbrOfSquares = current_.nbrOfSquares();
	for (int i = 0; i < nbrOfSquares; ++i) {
		bool filled = true;
		if (row >= 0) {
			int index = row * nbrColumns_;
			for (int column = 0; column < nbrColumns_; ++column) {
				if (gameboard_[index + column] == BlockType::EMPTY) {
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
	int indexStartOfRow = rowToRemove * nbrColumns_;
	// Copy all rows abowe the row to be removed.
	std::vector<BlockType> upperBoard(gameboard_.begin() + indexStartOfRow + nbrColumns_, gameboard_.end());
	// Erase the row to be removed and all rows abowe.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.end());
	// Insert the rows that were abowe the row to be removed.
	gameboard_.insert(gameboard_.end(), upperBoard.begin(), upperBoard.end());
	// Add a empty row at the top in order of replacing the row that were removed.
	gameboard_.resize(gameboard_.size() + nbrColumns_, BlockType::EMPTY);
}
