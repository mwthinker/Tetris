#include "block.h"

#include <algorithm>

Block::Block() : maxRotations_(0), currentRotation_(0), rotationSquareIndex_(0),
blockType_(BlockType::EMPTY), lowestStartRow_(0), startColumn_(0) {
}

Block::Block(BlockType blockType, int startColumn, int lowestStartRow, int currentRotation)
	: Block(blockType, startColumn, lowestStartRow) {
	
	for (int i = 0; i < currentRotation; ++i) {
		rotateLeft();
	}
}

Block::Block(BlockType blockType, int startColumn, int lowestStartRow) :
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
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 3);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 2);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		maxRotations_ = 1;
		break;
	case BlockType::J:
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_ + 2);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		break;
	case BlockType::L:
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 2);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_);
		break;
	case BlockType::O:
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_);
		maxRotations_ = 0;
		break;
	case BlockType::S:
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn - 1, lowestStartRow_);
		maxRotations_ = 1;
		break;
	case BlockType::T:
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn - 1, lowestStartRow_);
		break;
	case BlockType::Z:
		squares_[nbrOfSquares++] = Square(startColumn - 1, lowestStartRow_ + 1);
		squares_[nbrOfSquares++] = Square(startColumn , lowestStartRow_ + 1);
		rotationSquareIndex_ = nbrOfSquares;
		squares_[nbrOfSquares++] = Square(startColumn, lowestStartRow_);
		squares_[nbrOfSquares++] = Square(startColumn + 1, lowestStartRow_);
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

void Block::rotate(int rotate) {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;

	if (rotate > 0) {
		// Rotate left.
		for (int i = 0; i < rotate; ++i) {
			for (Square& sq : squares_) {
				Square tmp = sq;
				tmp.column_ = column + row - sq.row_;
				tmp.row_ = sq.column_ + row - column;
				sq = tmp;
			}
		}
	} else {
		rotate *= -1;
		for (int i = 0; i < rotate; ++i) {
			// Rotate right.
			for (Square& sq : squares_) {
				Square tmp = sq;
				tmp.column_ = sq.row_ + column - row;
				tmp.row_ = column + row - sq.column_;
				sq = tmp;
			}
		}
	}
}

void Block::rotateLeft() {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;
	currentRotation_ = (currentRotation_ + 1) % 4;

	// Rotate back to start orientation?
	if (currentRotation_ > maxRotations_) {
		currentRotation_ = 0;
		// Rotate right in order to get back to default orientation.
		rotate(-maxRotations_);
	} else {
		// Rotate to the left!
		rotate(1);
	}
}

void Block::rotateRight() {
	int row = squares_[rotationSquareIndex_].row_;
	int column = squares_[rotationSquareIndex_].column_;
	currentRotation_ = (currentRotation_ + 3) % 4;

	// Rotate back to start orientation?
	if (currentRotation_ > maxRotations_) {
		currentRotation_ = 0;
		rotate(maxRotations_);
	} else {
		// Rotate to the right!
		rotate(-1);
	}
}
