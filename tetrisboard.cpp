#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>
#include <map>

#include <random>

namespace {

	std::random_device rd;
	std::default_random_engine generator(rd());

}

TetrisBoard::TetrisBoard(int nbrOfRows, int nbrOfColumns, double newLineSquaresPercenters) {
	init(nbrOfRows,nbrOfColumns,newLineSquaresPercenters);
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
		case LEFT: {
			Block tmp = block;
			tmp.moveLeft();
			if (!collision(tmp)) {
				block = tmp;
				gameEvents_.push(PLAYER_MOVES_BLOCK_LEFT);
			}
			break;
				   }
		case RIGHT: {
			Block tmp = block;
			tmp.moveRight();
			if (!collision(tmp)) {
				block = tmp;
				gameEvents_.push(PLAYER_MOVES_BLOCK_RIGHT);
			}
			break;
					}
		case DOWN: {
			Block tmp = block;
			tmp.moveDown();
			if (!collision(tmp)) {
				block = tmp;
				gameEvents_.push(PLAYER_MOVES_BLOCK_DOWN);
			}
			break;
				   }
		case ROTATE_LEFT: {
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
				getBlockFromBoard(current_[i].row, current_[i].column) = current_.blockType();
			}

			// Add rows due to some external event.
			for (int i = 0; i < addRows_; ++i) {
				addRow();
			}
			addRows_ = 0;

			// Remove any filled rows on the gameboard.
			int nbr = removeFilledRows(current_);

			// Updates the user controlled block.
			current_ = next_;
			next_ = generateBlock();

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

void TetrisBoard::setNonRandomCurrentBlockType(BlockType blockType) {
	nonRandomCurrentBlock_ = blockType;
	if (!decideRandom_) {
		current_ = Block(blockType,nbrOfRows_+1,nbrOfColumns_/2);
	}
}

void TetrisBoard::setNonRandomNextBlockType(BlockType blockType) {
	nonRandomNextBlock_ = blockType;
	if (!decideRandom_) {
		next_ = Block(blockType,nbrOfRows_+1,nbrOfColumns_/2);
	}
}

BlockType TetrisBoard::getNonRandomCurrentBlockType() const {
	return nonRandomCurrentBlock_;
}

BlockType TetrisBoard::getNonRandomNextBlockType() const {
	return nonRandomNextBlock_;	
}

void TetrisBoard::setDecideRandomBlockType(bool decideRandom) {
	decideRandom_ = decideRandom;
}

bool TetrisBoard::isDecideRandomBlockType() const {
	return decideRandom_;
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
	addRows_ += blockTypes.size() / getNbrOfColumns();
	squaresToAdd_.insert(squaresToAdd_.end(),blockTypes.begin(), blockTypes.end());
}

std::vector<BlockType> TetrisBoard::generateRow() const {
	// Creates a row represented by a vector filled with true or false.
	// True means it is filled with a square. False means empty.
	// The percentage of true per row is squaresPerLength_.
	
	const unsigned int size = getNbrOfColumns();
	std::vector<bool> row(size);
	std::uniform_int_distribution<int> distribution(0,size-1);

	for (unsigned int i = 0; i <  size * squaresPerLength_; ++i) {
		int index = distribution(generator);
		unsigned int nbr = 0;
		while (nbr < size) {
			if (!row[ (index+nbr) % size]) {
				row[ (index+nbr) % size] = true;
				break;
			}
			++nbr;
		}
	}

	std::vector<BlockType> rows;

	for (unsigned int i = 0; i < size; ++i) {
		if (row[i]) {			
			rows.push_back(generateBlockType());
		} else {
			rows.push_back(BLOCK_TYPE_EMPTY);
		}
	}
	return rows;
}

// private

void TetrisBoard::init(int nbrOfRows, int nbrOfColumns, double newLineSquaresPercenters) {
	nbrOfRows_ = nbrOfRows;
	nbrOfColumns_ = nbrOfColumns;
	isGameOver_ = false;
	squaresPerLength_ = newLineSquaresPercenters;

	decideRandom_ = true; // Must be defined before call to generateBlock();
	current_ = generateBlock();
	next_ = generateBlock();
	addRows_ = 0;

	nonRandomCurrentBlock_ = BLOCK_TYPE_I;
	nonRandomNextBlock_ = BLOCK_TYPE_I;

	gameboard_ = std::vector<BlockType>((nbrOfRows+4)*(nbrOfColumns), BLOCK_TYPE_EMPTY);
}

void TetrisBoard::addRow() {
	std::vector<BlockType> upperBoard(squaresToAdd_.begin(), squaresToAdd_.end());
}

BlockType TetrisBoard::generateBlockType() const {
	BlockType blockType = nonRandomNextBlock_;
	std::uniform_int_distribution<int> distribution(0,6);

	if (decideRandom_) {	
		int nbr = distribution(generator);

		switch (nbr) {
		case 0:
			blockType = BLOCK_TYPE_I;
			break;
		case 1:
			blockType = BLOCK_TYPE_J;
			break;
		case 2:
			blockType = BLOCK_TYPE_L;
			break;
		case 3:
			blockType = BLOCK_TYPE_O;
			break;
		case 4:
			blockType = BLOCK_TYPE_S;
			break;
		case 5:
			blockType = BLOCK_TYPE_T;
			break;
		case 6:
			blockType = BLOCK_TYPE_Z;
			break;
		}
	}

	return blockType;
}

Block TetrisBoard::generateBlock() const {	
	return Block(generateBlockType(),nbrOfRows_,nbrOfColumns_/2 - 1);;
}

bool TetrisBoard::collision(Block block) const {
	bool collision = false;
	int nbrOfSquares = block.nbrOfSquares();

	for (int i = 0; i < nbrOfSquares; ++i) {
		Square square = block[i];
		if (square.row < 0) {
			collision = true;
			break;
		}
		if (square.column < 0 || square.column >= nbrOfColumns_) {
			collision = true;
			break;
		}
		if (getBlockFromBoard(block[i].row,block[i].column) != BLOCK_TYPE_EMPTY) {
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
			for (int column = 0; column < nbrOfColumns_; ++column) {
				if (getBlockFromBoard(row, column) == BLOCK_TYPE_EMPTY) {
					filled = false;
					break;
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
}
