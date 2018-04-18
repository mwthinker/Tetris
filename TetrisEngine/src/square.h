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
	Square() = default;

	Square(int column, int row)
		: column_(column), row_(row) {
	}

	bool operator==(const Square& square) const {
		return square.column_ == column_ && square.row_ == row_;
	}

	bool operator!=(const Square& square) const {
		return square.column_ != column_ || square.row_ != row_;
	}

	int column_, row_;
};

#endif	// SQUARE_H
