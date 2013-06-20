#include "block.h"

#include <algorithm>

Block::Block() {
	maxRotations_ = 0;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = BLOCK_TYPE_EMPTY;
	nbrOfSquares_ = 0;
	lowestRow_ = 0;
}

Block::Block(BlockType blockType, int bottomRow, int leftColumn) {
    maxRotations_ = 4;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = blockType;
	nbrOfSquares_ = 0;
    switch (blockType) {
	case BLOCK_TYPE_I:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 3, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 2, leftColumn);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		maxRotations_ = 1;
		lowestRow_ = bottomRow;
		break;
	case BLOCK_TYPE_J:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 2, leftColumn + 1);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn + 1);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		lowestRow_ = bottomRow;
		break;
	case BLOCK_TYPE_L:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 2, leftColumn);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn + 1);
		lowestRow_ = bottomRow;
		break;
	case BLOCK_TYPE_O:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn + 1);
		maxRotations_ = 0;
		lowestRow_ = bottomRow;
		break;
	case BLOCK_TYPE_S:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn - 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
	case BLOCK_TYPE_T:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn + 1);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn - 1);
		lowestRow_ = bottomRow - 1;
		break;
	case BLOCK_TYPE_Z:
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn - 1);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow + 1, leftColumn);
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares_++] = Square(blockType_, bottomRow, leftColumn + 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
    default:
        break;
    }
}

void Block::move() {
	moveDown();
}

void Block::moveLeft() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		--sq.column_;
	}
}

void Block::moveRight() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		++sq.column_;
	}
}

void Block::moveUp() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		++sq.row_;
	}
	++lowestRow_;
}

void Block::moveDown() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		--sq.row_;
	}
	--lowestRow_;
}

void Block::rotateLeft() {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;
	currentRotation_ = (currentRotation_ + 1) % 4;

	// Rotate back to start orientation?
	if (currentRotation_ > maxRotations_) {
		currentRotation_ = 0;
		for (int i = 0; i < maxRotations_; ++i) {
			rotateRight();
		}
	} else {
		// Rotate to the right!
		for (int i = 0; i != nbrOfSquares_; ++i) {
			Square sq = squares_[i];
			Square tmp = sq;

			tmp.column_ = column + row - sq.row_;
			tmp.row_ = sq.column_ + row - column;

			squares_[i] = tmp;
		}
	}
}

void Block::rotateRight() {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square sq = squares_[i];
		Square tmp = sq;

		tmp.column_ = sq.row_ + column - row;
		tmp.row_ = column + row - sq.column_;

		squares_[i] = tmp;
	}
}

Square Block::operator[](int index) const {
	return squares_[index];
}

int Block::nbrOfSquares() const {
	return nbrOfSquares_;
}

Square Block::getRotationSquare() const {
	return squares_[rotationSquareIndex_];
}

BlockType Block::blockType() const {
	return blockType_;
}

int Block::getLowestRow() const {
	return lowestRow_;
}
