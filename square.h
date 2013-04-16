#ifndef SQUARE_H
#define SQUARE_H

enum BlockType {BLOCK_TYPE_I,BLOCK_TYPE_J,BLOCK_TYPE_L,BLOCK_TYPE_O,
	BLOCK_TYPE_S, BLOCK_TYPE_T,BLOCK_TYPE_Z,BLOCK_TYPE_EMPTY};

struct Square {
	Square() {
		row = -1;
		column = -1;
	}

	Square(BlockType blockType, int row, int column) {
		this->row = row;
		this->column = column;
		this->blockType = blockType;
	}

	BlockType blockType;
	int row, column;

	bool operator==(const Square& square) const {
		return square.row == row && square.column == column;
	}

	bool operator!=(const Square& square) const {
		return !operator==(square);
	}
};

#endif	// SQUARE_H
