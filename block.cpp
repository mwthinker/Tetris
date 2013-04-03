#include "block.h"
#include <algorithm>

Block::Block() {
	maxRotations_ = 4;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = BLOCK_TYPE_I;
	nbrOfSquares_ = 0;
}

Block::Block(BlockType blockType, int bottomRow, int leftColumn) {
    maxRotations_ = 4;
	currentRotation_ = 0;
	rotationSquareIndex_ = 0;
	blockType_ = blockType;
	nbrOfSquares_ = 0;
    switch (blockType) {
	case BLOCK_TYPE_I:
		addSquare(bottomRow+3,leftColumn);
		addSquare(bottomRow+2,leftColumn);
		addRotationSquare(bottomRow+1,leftColumn);
		addSquare(bottomRow,leftColumn);
		setMaxRotations(1);
		break;
	case BLOCK_TYPE_J:
		addSquare(bottomRow+2,leftColumn+1);
		addRotationSquare(bottomRow+1,leftColumn+1);
		addSquare(bottomRow,leftColumn+1);
		addSquare(bottomRow,leftColumn);
		break;
	case BLOCK_TYPE_L:
		addSquare(bottomRow+2,leftColumn);
		addRotationSquare(bottomRow+1,leftColumn);
		addSquare(bottomRow,leftColumn);
		addSquare(bottomRow,leftColumn+1);
		break;
	case BLOCK_TYPE_O:
		addSquare(bottomRow+1,leftColumn);
		addSquare(bottomRow+1,leftColumn+1);
		addSquare(bottomRow,leftColumn);
		addSquare(bottomRow,leftColumn+1);
		setMaxRotations(0);
		break;
	case BLOCK_TYPE_S:
		addSquare(bottomRow+1,leftColumn+1);
		addSquare(bottomRow+1,leftColumn);
		addRotationSquare(bottomRow,leftColumn);
		addSquare(bottomRow,leftColumn-1);
		setMaxRotations(1);
		break;
	case BLOCK_TYPE_T:
		addSquare(bottomRow+1,leftColumn);
		addSquare(bottomRow,leftColumn+1);
		addRotationSquare(bottomRow,leftColumn);
		addSquare(bottomRow,leftColumn-1);
		break;
	case BLOCK_TYPE_Z:
		addSquare(bottomRow+1,leftColumn-1);
		addSquare(bottomRow+1,leftColumn);
		addRotationSquare(bottomRow,leftColumn);
		addSquare(bottomRow,leftColumn+1);
		setMaxRotations(1);
		break;
    default:
        break;
    }
}

bool Block::collision(Square square) const {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square sq = squares_[i];
		if (sq.row == square.row && sq.column == square.column) {
			return true;
		}
	}
	return false;
}

void Block::move() {
	moveDown();
}

void Block::moveLeft() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		--sq.column;
	}
}

void Block::moveRight() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		++sq.column;
	}
}

void Block::moveUp() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		++sq.row;
	}
}

void Block::moveDown() {
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square& sq = squares_[i];
		--sq.row;
	}
}

void Block::rotateLeft() {
	int row = squares_[rotationSquareIndex_].row;
	int column = squares_[rotationSquareIndex_].column;
	currentRotation_ = (currentRotation_ + 1) % 4;

	int rotations = 1;
	if (currentRotation_ > maxRotations_) {
		currentRotation_ = 0;
		for (int i = 0; i < maxRotations_; ++i) {//rotations
			rotateRight();
		}
		rotations = 0;
	}

	for (int i = 0; i < rotations; ++i) {
		for (int i = 0; i != nbrOfSquares_; ++i) {
			Square sq = squares_[i];
			Square tmp = sq;

			tmp.column = column + row - sq.row;
			tmp.row = sq.column + row - column;

			squares_[i] = tmp;
		}
	}
}

void Block::rotateRight() {
	int row = squares_[rotationSquareIndex_].row;
	int column = squares_[rotationSquareIndex_].column;
	for (int i = 0; i != nbrOfSquares_; ++i) {
		Square sq = squares_[i];
		Square tmp = sq;

		tmp.column = sq.row + column - row;
		tmp.row = column + row - sq.column;

		squares_[i] = tmp;
	}
}

bool Block::operator==(const Block& block) const {
	if (block.nbrOfSquares_ != nbrOfSquares_) {
		for (int i = 0; i != nbrOfSquares_; ++i) {
			if (squares_[i] != block.squares_[i]) {
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Block::operator!=(const Block& block) const {
	return !operator==(block);
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

void Block::addSquare(int row, int colon) {
	if ( std::find(squares_,squares_ + nbrOfSquares_,Square(blockType_,row,colon)) == squares_ + nbrOfSquares_ ) {
		squares_[nbrOfSquares_++] = Square(blockType_,row,colon);
	}
}

void Block::addRotationSquare(int row, int colon) {
	if ( std::find(squares_,squares_ + nbrOfSquares_,Square(blockType_,row,colon)) == squares_ + nbrOfSquares_ ) {
		rotationSquareIndex_ = nbrOfSquares_;
		squares_[nbrOfSquares_++] = Square(blockType_,row,colon);
	}
}

// Private methods

void Block::setMaxRotations(int nbr) {
	maxRotations_ = nbr;
}
