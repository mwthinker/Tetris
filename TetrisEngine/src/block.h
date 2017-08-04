#ifndef BLOCK_H
#define BLOCK_H

#include "square.h"

#include <array>

class Block {
public:
	using const_iterator = std::array<Square, 4>::const_iterator;

    Block();
	Block(BlockType blockType, int lowestRow, int startColumn);
	Block(BlockType blockType, int lowestRow, int startColumn, int rotations);
	
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void rotateLeft();
	void rotateRight();
	
	Square operator[](int index) const {
		return squares_[index];
	}

	int getSize() const {
		return squares_.size();
	}

	Square getRotationSquare() const {
		return squares_[rotationSquareIndex_];
	}

	BlockType getBlockType() const {
		return blockType_;
	}

	// Returns the lowest possible row for the block when rotating.
	int getLowestRow() const {
		return lowestRow_;
	}	

	const_iterator begin() const {
		return squares_.begin();
	}

	const_iterator end() const {
		return squares_.end();
	}

	int getNumberOfRotations() const {
		return maxRotations_;
	}

	int getCurrentRotation() const {
		return currentRotation_;
	}

	int getStartColumn() const {
		return startColumn_;
	}

private:
	int rotationSquareIndex_;
	std::array<Square, 4> squares_;
	int maxRotations_, currentRotation_;
	BlockType blockType_;
	int lowestRow_;
	int startColumn_;
};

#endif	// BLOCK_H
