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
	
	inline Square operator[](int index) const {
		return squares_[index];
	}

	inline int getSize() const {
		return squares_.size();
	}

	inline Square getRotationSquare() const {
		return squares_[rotationSquareIndex_];
	}

	inline BlockType getBlockType() const {
		return blockType_;
	}

	// Returns the lowest possible row for the block when rotating.
	inline int getLowestRow() const {
		return lowestRow_;
	}	

	inline const_iterator begin() const {
		return squares_.begin();
	}

	inline const_iterator end() const {
		return squares_.end();
	}

	inline int getNumberOfRotations() const {
		return maxRotations_;
	}

	inline int getCurrentRotation() const {
		return currentRotation_;
	}

	inline int getStartColumn() const {
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
