#ifndef BLOCK_H
#define BLOCK_H

#include "square.h"

#include <vector>

class Block {
public:
    Block();
	Block(BlockType blockType, int bottomRow, int leftColumn);

	void move();
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

private:
	int rotationSquareIndex_;
	Square squares_[4];
	int nbrOfSquares_;
	int maxRotations_, currentRotation_;
	BlockType blockType_;
	int lowestRow_;
};

#endif	// BLOCK_H
