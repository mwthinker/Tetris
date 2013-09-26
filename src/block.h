#ifndef BLOCK_H
#define BLOCK_H

#include "square.h"

#include <array>

class Block {
public:
    Block();
	Block(BlockType blockType, int bottomRow, int leftColumn);
	
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void rotateLeft();
	void rotateRight();
	
	Square operator[](int index) const;
	int nbrOfSquares() const;

	Square getRotationSquare() const;
	BlockType blockType() const;
	
	// Returns the lowest possible row for the block when rotating.
	int getLowestRow() const;

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

private:
	int rotationSquareIndex_;
	std::array<Square, 4> squares_;
	int maxRotations_, currentRotation_;
	BlockType blockType_;
	int lowestRow_;
};

#endif	// BLOCK_H
