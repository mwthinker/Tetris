#ifndef SQUARE_H
#define SQUARE_H

enum class BlockType {
	I,
	J,
	L,
	O,
	S,
	T,
	Z,
	EMPTY,
	WALL
};

class Square {
public:
	Square() {
	}

	Square(BlockType blockType, int row, int column) : blockType_(blockType), row_(row), column_(column) {
	}

	bool operator==(const Square& square) const {
		return square.row_ == row_ && square.column_ == column_;
	}

	bool operator!=(const Square& square) const {
		return square.row_ != row_ || square.column_ != column_;
	}

	BlockType blockType_;
	int row_, column_;
};

#endif	// SQUARE_H
