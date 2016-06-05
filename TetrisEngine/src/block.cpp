#include "block.h"

#include <algorithm>

Block::Block() {
	maxRotations_ = 0;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = BlockType::EMPTY;
	lowestRow_ = 0;
}

Block::Block(BlockType blockType, int bottomRow, int leftColumn) {
    maxRotations_ = 4;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = blockType;
	int nbrOfSquares = 0;
    switch (blockType) {
	case BlockType::EMPTY:
		maxRotations_ = 0;
		break;
	case BlockType::I:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 3, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, leftColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		maxRotations_ = 1;
		lowestRow_ = bottomRow;
		break;
	case BlockType::J:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, leftColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		lowestRow_ = bottomRow;
		break;
	case BlockType::L:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, leftColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn + 1);
		lowestRow_ = bottomRow;
		break;
	case BlockType::O:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn + 1);
		maxRotations_ = 0;
		lowestRow_ = bottomRow;
		break;
	case BlockType::S:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn - 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
	case BlockType::T:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn - 1);
		lowestRow_ = bottomRow - 1;
		break;
	case BlockType::Z:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn - 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, leftColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, leftColumn + 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
    }
}

void Block::moveLeft() {
	for (Square& sq : squares_) {
		--sq.column_;
	}
}

void Block::moveRight() {
	for (Square& sq : squares_) {
		++sq.column_;
	}
}

void Block::moveUp() {
	for (Square& sq : squares_) {
		++sq.row_;
	}
	++lowestRow_;
}

void Block::moveDown() {
	for (Square& sq : squares_) {
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
		for (Square& sq : squares_) {
			Square tmp = sq;
			tmp.column_ = column + row - sq.row_;
			tmp.row_ = sq.column_ + row - column;
			sq = tmp;
		}
	}
}

void Block::rotateRight() {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;
	for (Square& sq : squares_) {
		Square tmp = sq;
		tmp.column_ = sq.row_ + column - row;
		tmp.row_ = column + row - sq.column_;
		sq = tmp;
	}
}

Square Block::operator[](int index) const {
	return squares_[index];
}

int Block::nbrOfSquares() const {
	return squares_.size();
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
