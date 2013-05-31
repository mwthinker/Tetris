#include "tetrisboard.h"

#include "square.h"
#include "block.h"

#include <vector>
#include <queue>
#include <algorithm>
#include <iostream>

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
				gameboard_.push_back(current_[i]);
			}

			// Add rows due to some external event.
			for (int i = 0; i < addRows_; ++i) {
				addRow();
			}
			addRows_ = 0;

			// Updates the user controlled block.
			current_ = next_;
			next_ = generateBlock();

			// Remove any filled rows on the gameboard.
			int nbr = removeFilledRows();

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

const Squares& TetrisBoard::getGameBoard() const {
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
}

void TetrisBoard::addRow() {
	// Moves all current rows up one step in order to make rome to add one row at the bottom.
	for (std::vector<Square>::iterator it = gameboard_.begin(); it != gameboard_.end(); ++it) {
		Square& square = *it;
		square.row += 1;
	}

	const unsigned int size = getNbrOfColumns();
	if (squaresToAdd_.size() > 0) {
		// Adds the square to the row
		for (unsigned int i = 0; i < size; ++i) {
			BlockType type = squaresToAdd_[i];
			if (type != BLOCK_TYPE_EMPTY) {
				Square square(type,1,i+1);
				fillRow(square);
			}
		}

		// Removes the added squares from the add row vector.
		if (squaresToAdd_.size() >= 2*size) {
			std::vector<BlockType> tmp(squaresToAdd_.begin()+size,squaresToAdd_.end());
			squaresToAdd_ = tmp;
		} else {
			squaresToAdd_.clear();
		}
	}
}

void TetrisBoard::fillRow(const Square& square) {
	std::vector<Square>::iterator it = std::find(gameboard_.begin(),gameboard_.end(),square);
	if (it != gameboard_.end()) {
		*it = square;
	} else {
		gameboard_.push_back(square);
	}
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
	return Block(generateBlockType(),nbrOfRows_+1,nbrOfColumns_/2);;
}

bool TetrisBoard::collision(Block block) const {
	bool collision = false;
	// checks if the block is inside any square on the gameboard
	for (std::vector<Square>::const_iterator it = gameboard_.begin(); it != gameboard_.end(); ++it) {
		Square square = *it;
		collision = block.collision(square);
		if (collision) {
			break;
		}
	}

	// checks if the block is outside the gameboard
	int nbrOfSquares = block.nbrOfSquares();

	for (int i = 0; i < nbrOfSquares; ++i) {
		Square square = block[i];
		if (square.row < 1) {
			collision = true;
			break;
		}
		if (square.column < 1 || square.column > nbrOfColumns_) {
			collision = true;
			break;
		}
	}

	return collision;
}

int TetrisBoard::removeFilledRows() {
	std::vector<int> filledRows(nbrOfRows_+1);

	// Counts the number of squares in each row
	for (std::vector<Square>::iterator it = gameboard_.begin(); it != gameboard_.end(); ++it) {
		Square square = *it;
		// Will enter every time except for the last block when the game becomes game over
		if (square.row < nbrOfRows_ + 1) {
			++filledRows[square.row];
		}
	}

	// Filled rows removed and rows abowe are moved down to fill the gap
	int nbr = 0;
	for (int row = 1; row < nbrOfRows_ + 1; ++row) {
		if (filledRows[row] == nbrOfColumns_) {
			removeSquares(row-nbr);
			moveRowsOneStepDown(row-nbr);
			++nbr;
		}
	}

	return nbr;
}

void TetrisBoard::removeSquares(int row) {
	int nbr = 0;
	int size = gameboard_.size();
	// find all squares in the specified row puts them last in the vector
	for (int i = 0; i < size - nbr; ++i) {
		Square& square = gameboard_[i];
		if (square.row == row) {
			std::swap(square, gameboard_[size-nbr-1]);
			++nbr;
			--i;
		}
		if (nbr == size) {
			break;
		}
	}
	// all the squares in the specified row is removed
	for (int i = 0; i < nbr; ++i) {
		gameboard_.pop_back();
	}
}

void TetrisBoard::moveRowsOneStepDown(int highestRow) {
	for (std::vector<Square>::iterator it = gameboard_.begin(); it != gameboard_.end(); ++it) {
		Square& square = *it;
		if (square.row >= highestRow) {
			--square.row;
		}
	}
}