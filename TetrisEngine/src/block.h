#ifndef BLOCK_H
#define BLOCK_H

#include "square.h"

#include <array>

class Block {
public:
    Block();
	Block(BlockType blockType, int lowestRow, int leftColumn);
	Block(BlockType blockType, int lowestRow, int leftColumn, int rotations);
	
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void rotateLeft();
	void rotateRight();
	
	inline Square Block::operator[](int index) const {
		return squares_[index];
	}

	inline int Block::nbrOfSquares() const {
		return squares_.size();
	}

	inline Square Block::getRotationSquare() const {
		return squares_[rotationSquareIndex_];
	}

	inline BlockType Block::getBlockType() const {
		return blockType_;
	}

	// Returns the lowest possible row for the block when rotating.
	inline int Block::getLowestRow() const {
		return lowestRow_;
	}	

	inline std::array<Square, 4>::const_iterator begin() const {
		return squares_.begin();
	}

	inline std::array<Square, 4>::const_iterator end() const {
		return squares_.end();
	}

	inline int getNumberOfRotations() const {
		return maxRotations_;
	}

	inline int getCurrentRotation() const {
		return currentRotation_;
	}

	inline int getLeftColumn() const {
		return leftColumn_;
	}

private:
	int rotationSquareIndex_;
	std::array<Square, 4> squares_;
	int maxRotations_, currentRotation_;
	BlockType blockType_;
	int lowestRow_;
	int leftColumn_;
};

#endif	// BLOCK_H
