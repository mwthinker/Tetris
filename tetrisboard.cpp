#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>

TetrisBoard::TetrisBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next) {
	nbrOfRows_ = nbrOfRows;
	nbrOfColumns_ = nbrOfColumns;
	isGameOver_ = false;

	current_ = createBlock(current);
	next_ = createBlock(next);

	gameboard_ = std::vector<BlockType>( (nbrOfRows + 4) * (nbrOfColumns) , BLOCK_TYPE_EMPTY);
	rowsFilled_ = std::vector<int>(nbrOfRows + 4, 0);
}

void TetrisBoard::restart(BlockType current, BlockType next) {
	nextBlockQueue_ = std::queue<BlockType>();
	gameEvents_ = std::queue<GameEvent>();
	current_ = createBlock(current);
	next_ = createBlock(next);
	gameboard_ = std::vector<BlockType>( (nbrOfRows_ + 4) * nbrOfColumns_ , BLOCK_TYPE_EMPTY);
	rowsFilled_ = std::vector<int>(nbrOfRows_ + 4, 0);
	isGameOver_ = false;
}

// First checks if the game is over. Then perform the move.
void TetrisBoard::update(Move move) {
	// Check if gameover
	if (collision(current_)) {
		triggerGameOverEvent();
	}

	if (!isGameOver_) {
		Block block = current_;

		// Gravity move
		bool gravityCollision = false;

		switch (move) {
		case DOWN_GRAVITY:
			block.move();
			gravityCollision = collision(block);
			if (!gravityCollision) {
				gameEvents_.push(GRAVITY_MOVES_BLOCK);
			}
			break;
		case LEFT:
			{
				Block tmp = block;
				tmp.moveLeft();
				if (!collision(tmp)) {
					block = tmp;
					gameEvents_.push(PLAYER_MOVES_BLOCK_LEFT);
				}
				break;
			}
		case RIGHT:
			{
				Block tmp = block;
				tmp.moveRight();
				if (!collision(tmp)) {
					block = tmp;
					gameEvents_.push(PLAYER_MOVES_BLOCK_RIGHT);
				}
				break;
			}
		case DOWN:
			{
				Block tmp = block;
				tmp.moveDown();
				if (!collision(tmp)) {
					block = tmp;
					gameEvents_.push(PLAYER_MOVES_BLOCK_DOWN);
				}
				break;
			}
		case ROTATE_LEFT:
			{
				Block tmp = block;
				tmp.rotateLeft();
				if (!collision(tmp)) {
					block = tmp;
					gameEvents_.push(PLAYER_ROTATES_BLOCK);
				}
				break;
			}
		default:
			break;
		}

		if (gravityCollision) {
			// Collision detected, add squares to gameboard.
			int nbrOfSquares = current_.nbrOfSquares();
			for (int i = 0; i < nbrOfSquares; ++i) {
				getBlockFromBoard(current_[i].row_, current_[i].column_) = current_.blockType();
			}
			
			// Remove any filled rows on the gameboard.
			int nbr = removeFilledRows(current_);

			// Add rows due to some external event.
			addExternalRows();

			// Updates the user controlled block.
			current_ = next_;
			next_ = createBlock(nextBlockQueue_.front());
			nextBlockQueue_.pop();

			// Add Game event
			gameEvents_.push(BLOCK_COLLISION);

			switch (nbr) {
			case 1:
				gameEvents_.push(ONE_ROW_REMOVED);
				break;
			case 2:
				gameEvents_.push(TWO_ROW_REMOVED);
				break;
			case 3:
				gameEvents_.push(THREE_ROW_REMOVED);
				break;
			case 4:
				gameEvents_.push(FOUR_ROW_REMOVED);
				break;
			}
		} else {
			// no collision, its journey can continue.
			current_ = block;
		}
	}
}

void TetrisBoard::add(BlockType next) {
	nextBlockQueue_.push(next);
}

void TetrisBoard::triggerGameOverEvent() {
	if (!isGameOver_) {
		gameEvents_.push(GAME_OVER);
		isGameOver_ = true;
	}
}

int TetrisBoard::getNbrOfRows() const {
	return nbrOfRows_;
}

int TetrisBoard::getNbrOfColumns() const {
	return nbrOfColumns_;
}

bool TetrisBoard::isGameOver() const {
	return isGameOver_;
}

const std::vector<BlockType>& TetrisBoard::getGameBoard() const {
	return gameboard_;
}

Block TetrisBoard::currentBlock() const {
	return current_;
}

Block TetrisBoard::nextBlock() const {
	return next_;
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

void TetrisBoard::addExternalRows() {
	gameboard_.insert(gameboard_.begin(), squaresToAdd_.begin(), squaresToAdd_.end());
	gameboard_.resize(gameboard_.size() - squaresToAdd_.size(), BLOCK_TYPE_EMPTY);
	squaresToAdd_.clear();
}

Block TetrisBoard::createBlock(BlockType blockType) const {
	return Block(blockType,nbrOfRows_,nbrOfColumns_/2 - 1);
}

bool TetrisBoard::collision(Block block) const {
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
		if (getBlockFromBoard(block[i].row_, block[i].column_) != BLOCK_TYPE_EMPTY) {
			collision = true;
			break;
		}
	}
	
	return collision;
}

BlockType TetrisBoard::getBlockFromBoard(int row, int column) const {
	return gameboard_[row*nbrOfColumns_ + column];
}

BlockType& TetrisBoard::getBlockFromBoard(int row, int column) {
	return gameboard_[row*nbrOfColumns_ + column];
}

int TetrisBoard::removeFilledRows(const Block& block) {
	int row = block.getLowestRow();

	int nbr = 0;
	int nbrOfSquares = current_.nbrOfSquares();
	for (int i = 0; i < 4; ++i) {
		bool filled = true;
		if (row >= 0) {
			rowsFilled_[row] = 0;
			for (int column = 0; column < nbrOfColumns_; ++column) {
				if (getBlockFromBoard(row, column) == BLOCK_TYPE_EMPTY) {
					filled = false;
					++rowsFilled_[row];
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

void TetrisBoard::moveRowsOneStepDown(int rowToRemove) {
	int indexStartOfRow = rowToRemove * nbrOfColumns_;
	// Copy all rows abowe the row to be removed.
	std::vector<BlockType> upperBoard(gameboard_.begin() + indexStartOfRow + nbrOfColumns_, gameboard_.end());
	// Erase the row to be removed and all rows abowe.
	gameboard_.erase(gameboard_.begin() + indexStartOfRow, gameboard_.end());
	// Insert the rows that were abowe the row to be removed.
	gameboard_.insert(gameboard_.end(), upperBoard.begin(), upperBoard.end());
	// Add a empty row at the top in order of replacing the row that were removed.
	gameboard_.resize(gameboard_.size() + nbrOfColumns_,BLOCK_TYPE_EMPTY);

	// Move down.
	rowsFilled_.erase(rowsFilled_.begin() + rowToRemove);
	// Replace the removed row.
	rowsFilled_.push_back(0);
}
