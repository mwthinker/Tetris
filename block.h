#ifndef BLOCK_H
#define BLOCK_H

#include "square.h"

#include <vector>

class Block {
public:
    Block();
	Block(BlockType blockType, int bottomRow, int leftColumn);	

	bool collision(Square square) const ;

	void move();
	void moveLeft();
	void moveRight();
	void moveUp();
	void moveDown();
	void rotateLeft();
	void rotateRight();

	bool operator==(const Block& block) const;
	bool operator!=(const Block& block) const;
	
	Square operator[](int index) const;
	int nbrOfSquares() const;

	Square getRotationSquare() const;
	BlockType blockType() const;
private:
    //typedef std::vector<Square>::iterator square_iterator;
    //typedef std::vector<Square>::const_iterator square_const_iterator;

    void addSquare(int row, int colon);
	void addRotationSquare(int row, int colon);
	void setMaxRotations(int nbr);

	int rotationSquareIndex_;
	//std::vector<Square> squares_;
	Square squares_[10];
	int nbrOfSquares_;
	int maxRotations_, currentRotation_;
	BlockType blockType_;
};

#endif	// BLOCK_H
