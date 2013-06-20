#ifndef SQUARE_H
#define SQUARE_H

enum BlockType {
	BLOCK_TYPE_I,
	BLOCK_TYPE_J,
	BLOCK_TYPE_L,
	BLOCK_TYPE_O,
	BLOCK_TYPE_S,
	BLOCK_TYPE_T,
	BLOCK_TYPE_Z,
	BLOCK_TYPE_EMPTY
};

class Square {
public:
	inline Square() {
	}

	inline Square(BlockType blockType, int row, int column) : blockType_(blockType), row_(row), column_(column) {
	}

	inline bool operator==(const Square& square) const {
		return square.row_ == row_ && square.column_ == column_;
	}

	inline bool operator!=(const Square& square) const {
		return square.row_ != row_ || square.column_ != column_;
	}

	BlockType blockType_;
	int row_, column_;
};

#endif	// SQUARE_H
