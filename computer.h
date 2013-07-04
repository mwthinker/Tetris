#ifndef CUMPUTER_H
#define CUMPUTER_H

#include "device.h"
#include "tetrisboard.h"

class Computer {
public:
	Computer(const TetrisBoard& board) : board_(board) {
	}

	// Find all possible positions for the block.
	void update(const Block& current) {
		int maxRow = board_.getNbrOfRows();
		for (int row = 0; row < maxRow; ++row) {
			if (board_.nbrOfSquares(row) == 0) {
				maxRow = row;
				break;
			}
		}
		

	}

private:
	const TetrisBoard& board_;
};

#endif // CUMPUTER_H