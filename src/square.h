#ifndef SQUARE_H
#define SQUARE_H

enum class BlockType : char {
	I,
	J,
	L,
	O,
	S,
	T,
	Z,
	EMPTY
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
