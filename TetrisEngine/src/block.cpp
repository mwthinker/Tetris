#include "block.h"

#include <algorithm>

Block::Block() : maxRotations_(0), currentRotation_(0), rotationSquareIndex_(0),
blockType_(BlockType::EMPTY), lowestStartRow_(0), startColumn_(0) {
}

Block::Block(BlockType blockType, int lowestStartRow, int leftColumn, int currentRotation)
	: Block(blockType, lowestStartRow, leftColumn) {
	
	for (int i = 0; i < currentRotation; ++i) {
		rotateLeft();
	}
}

Block::Block(BlockType blockType, int lowestStartRow, int startColumn) : 
	maxRotations_(4), currentRotation_(0), rotationSquareIndex_(0), 
	blockType_(blockType), startColumn_(startColumn), lowestStartRow_(lowestStartRow) {

	int nbrOfSquares = 0;
    switch (blockType) {
	case BlockType::EMPTY:
		maxRotations_ = 0;
		squares_[nbrOfSquares++] = Square(0, 0);
		squares_[nbrOfSquares++] = Square(0, 0);
		squares_[nbrOfSquares++] = Square(0, 0);
		squares_[nbrOfSquares++] = Square(0, 0);
		break;
	case BlockType::I:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 3, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 2, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		maxRotations_ = 1;
		break;
	case BlockType::J:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 2, startColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn + 1);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		break;
	case BlockType::L:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 2, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn + 1);
		break;
	case BlockType::O:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn + 1);
		maxRotations_ = 0;
		break;
	case BlockType::S:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn + 1);
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn - 1);
		maxRotations_ = 1;
		break;
	case BlockType::T:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn - 1);
		break;
	case BlockType::Z:
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn - 1);
		squares_[nbrOfSquares++] = Square(lowestStartRow_ + 1, startColumn);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn);
		squares_[nbrOfSquares++] = Square(lowestStartRow_, startColumn + 1);
		maxRotations_ = 1;
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
	++lowestStartRow_;
	for (Square& sq : squares_) {
		++sq.row_;
	}
}

void Block::moveDown() {
	--lowestStartRow_;
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
	currentRotation_ = (currentRotation_ + 3) % 4;
	for (Square& sq : squares_) {
		Square tmp = sq;
		tmp.column_ = sq.row_ + column - row;
		tmp.row_ = column + row - sq.column_;
		sq = tmp;
	}
}
