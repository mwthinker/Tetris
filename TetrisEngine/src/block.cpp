#include "block.h"

#include <algorithm>

Block::Block() : maxRotations_(0), currentRotation_(0), rotationSquareIndex_(0),
blockType_(BlockType::EMPTY), lowestRow_(0), startColumn_(0) {
	squares_[0] = Square(blockType_, 0, 0);
	squares_[1] = Square(blockType_, 0, 0);
	squares_[2] = Square(blockType_, 0, 0);
	squares_[3] = Square(blockType_, 0, 0);
}

Block::Block(BlockType blockType, int bottomRow, int leftColumn, int currentRotation) : Block(blockType, bottomRow, leftColumn) {
	for (int i = 0; i < currentRotation; ++i) {
		rotateLeft();
	}
}

Block::Block(BlockType blockType, int bottomRow, int startColumn) : maxRotations_(4),
currentRotation_(0), rotationSquareIndex_(0), blockType_(blockType), startColumn_(startColumn) {

	int nbrOfSquares = 0;
    switch (blockType) {
	case BlockType::EMPTY:
		maxRotations_ = 0;
		squares_[nbrOfSquares++] = Square(blockType_, 0, 0);
		squares_[nbrOfSquares++] = Square(blockType_, 0, 0);
		squares_[nbrOfSquares++] = Square(blockType_, 0, 0);
		squares_[nbrOfSquares++] = Square(blockType_, 0, 0);
		break;
	case BlockType::I:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 3, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		maxRotations_ = 1;
		lowestRow_ = bottomRow;
		break;
	case BlockType::J:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, startColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		lowestRow_ = bottomRow;
		break;
	case BlockType::L:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 2, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn + 1);
		lowestRow_ = bottomRow;
		break;
	case BlockType::O:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn + 1);
		maxRotations_ = 0;
		lowestRow_ = bottomRow;
		break;
	case BlockType::S:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn - 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
	case BlockType::T:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn - 1);
		lowestRow_ = bottomRow - 1;
		break;
	case BlockType::Z:
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn - 1);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow + 1, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn);
		squares_[nbrOfSquares++] = Square(blockType_, bottomRow, startColumn + 1);
		maxRotations_ = 1;
		lowestRow_ = bottomRow - 1;
		break;
    }
}

void Block::moveLeft() {
	--startColumn_;
	for (Square& sq : squares_) {
		--sq.column_;
	}
}

void Block::moveRight() {
	++startColumn_;
	for (Square& sq : squares_) {
		++sq.column_;
	}
}

void Block::moveUp() {
	++lowestRow_;
	for (Square& sq : squares_) {
		++sq.row_;
	}
}

void Block::moveDown() {
	--lowestRow_;
	for (Square& sq : squares_) {
		--sq.row_;
	}
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
