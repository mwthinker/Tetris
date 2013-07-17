#include "computer.h"
#include "tetrisboard.h"

#include <vector>
#include <string>

Computer::Computer() {
	latestId_ = -1;
}

Input Computer::currentInput() {
	return input_;
}

std::string Computer::getName() const {
	return "Computer";
}

void Computer::update(const TetrisBoard& board) {
	// New block appears?
	if (latestId_ != board.getNbrOfUpdates()) {
		latestId_ = board.getNbrOfUpdates();
		latestState_ = calculateBestState(board);
		input_ = calculateInput(latestState_);
		latestBlock_ = board.currentBlock();
	} else {
		Block current = board.currentBlock();
		Square currentSq = current.getRotationSquare();
		Square sq = latestBlock_.getRotationSquare();

		if (latestState_.left_ == sq.column_ - currentSq.column_) {
			latestState_.left_ = 0;
		}

		if (latestState_.rotations_ == current.getCurrentRotation() - latestBlock_.getCurrentRotation()) {
			latestState_.rotations_ = 0;
		}

		input_ = calculateInput(latestState_);
	}
}

double Computer::calculateValue(const TetrisBoard& board) const {
	int value = 0;
	const int MAXROWS = board.getNbrOfRows();
	for (int row = 0; row < MAXROWS; ++row) {
		//int nbr = board.nbrOfSquares(row);
		//value -= nbr * row * 0;
	}

	int lowestRow = board.getNbrOfRows() +4;
	int nbrOfHoles = 0;

	double rowRoughness = 0;
	for (int row = 0; row < lowestRow; ++row) {
		BlockType lastType;
		for (int column = 0; column < board.getNbrOfColumns(); ++column) {
			BlockType type = board.getBlockFromBoard(row, column);
			if (column == 0) {
				lastType = type;
			} else {
				if (lastType != type) {
					rowRoughness += -5;
				}
				lastType = type;
			}
		}
	}

	double columnRoughness = 0;	
	for (int column = 0; column < board.getNbrOfColumns(); ++column) {
		BlockType lastType;
		for (int row = 0; row < lowestRow; ++row) {			
			BlockType type = board.getBlockFromBoard(row, column);
			if (row == 0) {
				lastType = type;
			} else {
				if (lastType != type) {
					columnRoughness += -10;
				}
				lastType = type;
			}
		}
	}

	return value + rowRoughness + columnRoughness - lowestRow * 10;
}

// Calculate and return the best input to achieve the current state.
Input Computer::calculateInput(State state) const {
	Input input;
	if (state.rotations_ > 0) {
		input.rotate_ = !input_.rotate_;
	}
	if (state.left_ > 0) {
		input.left_ = true;
	} else if (state.left_ < 0) {
		input.right_ = true;
	}
	if (state.left_ == 0 && state.rotations_ == 0) {
		input.down_ = true;
	}
	return input;
}

// Find the best state for the block to move.
Computer::State Computer::calculateBestState(const TetrisBoard& board) {
	std::vector<State> states = calculateAllPossibleStates(board, board.currentBlock());

	double highestValue = -100000;
	int hIndex = -1;
	for (unsigned int index = 0; index < states.size(); ++index) {
		State state = states[index];
		TetrisBoard childBoard = board;
		childBoard.add(BlockType::EMPTY);

		for (int i = 0; i < state.rotations_; ++i) {
			childBoard.update(TetrisBoard::Move::ROTATE_LEFT);
		}

		while (state.left_ != 0) {
			if (state.left_ < 0) {
				childBoard.update(TetrisBoard::Move::RIGHT);
				++state.left_;
			} else if (state.left_ > 0) {
				childBoard.update(TetrisBoard::Move::LEFT);
				--state.left_;
			}
		}

		for (int i = 0; i < state.down_-1; ++i) {
			childBoard.update(TetrisBoard::Move::DOWN_GRAVITY);
		}
		const TetrisBoard& tmp = childBoard;
		Block block = childBoard.currentBlock();
		int edges = 0;
		for (const Square& sq : block) {
			tmp.getBlockFromBoard(sq.row_-1, sq.column_-1) != BlockType::EMPTY ? ++edges : 0;
			tmp.getBlockFromBoard(sq.row_-1, sq.column_+1) != BlockType::EMPTY ? ++edges : 0;
		}
		childBoard.update(TetrisBoard::Move::DOWN_GRAVITY);

		double value = calculateValue(childBoard) + edges;

		if (value > highestValue) {
			hIndex = index;
			highestValue = value;
		}
	}

	if (hIndex >= 0) {
		return states[hIndex];
	}
	return State();
}

// Calculates and returns all posible states from the point of view from the block provided.
// The algortihm rotates and goes left or rigth with the block which then fall to the ground.
// I.e. The possible state.
std::vector<Computer::State> Computer::calculateAllPossibleStates(const TetrisBoard& board, Block block) const {
	std::vector<State> states;
	// Valid block position?
	if (!board.collision(block)) {
		State state;

		// Go through all rotations for the block.
		for (int rotations = 0; rotations <= block.getNumberOfRotations(); ++rotations, block.rotateLeft()) {
			// Go left.
			Block horizontal = block;
			horizontal.moveLeft();

			int stepsLeft = 1;
			// Go left until obstacle.
			while (!board.collision(horizontal)) {
				Block vertical = horizontal;
				int stepsDown = 1;
				vertical.moveDown();
				while (!board.collision(vertical)) {
					++stepsDown;
					vertical.moveDown();
				}
				states.push_back(State(stepsDown, stepsLeft, rotations));

				++stepsLeft;
				horizontal.moveLeft();
			}

			stepsLeft = 0;

			horizontal = block;
			// Go right until obstacle.
			while (!board.collision(horizontal)) {
				Block vertical = horizontal;
				int stepsDown = 1;
				vertical.moveDown();
				while (!board.collision(vertical)) {
					++stepsDown;
					vertical.moveDown();
				}
				states.push_back(State(stepsDown, stepsLeft, rotations));

				--stepsLeft;
				horizontal.moveRight();
			}
		}
	}
	return states;
}
