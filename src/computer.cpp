#include "computer.h"
#include "tetrisboard.h"

#include <vector>
#include <string>
#include <future>

Computer::Computer() : Device(true) {
	latestId_ = -1;
	activeThread_ = false;
}

Computer::Computer(const Ai& ai) : Device(true) {
	latestId_ = -1;
	ai_ = ai;
	activeThread_ = false;
	playerName_ = ai.getName();
}

Input Computer::currentInput() {
	return input_;
}

std::string Computer::getName() const {
	return "Computer";
}

std::string Computer::getPlayerName() const {
	return playerName_;
}

void Computer::update(const TetrisBoard& board) {
	// New block appears?
	if (latestId_ != board.getNbrOfUpdates() && !activeThread_) {
		activeThread_ = true;
		input_ = Input();
		latestId_ = board.getNbrOfUpdates();
		handle_ = std::async(std::launch::async|std::launch::deferred, [&] {
			return calculateBestState(board, 2);
		});
	} else {
		if (handle_.valid()) {
			latestState_ = handle_.get();
			latestBlock_ = board.currentBlock();
			input_ = calculateInput(latestState_);
			handle_ = std::future<State>();
			activeThread_ = false;
		}
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

double Computer::calculateValue(const RawTetrisBoard& board, const Block& block) const {
	int lowestRow = board.getNbrOfRows() + 4;
	double meanHeight = 0;
	int nbrOfSquares = 0;

	double rowRoughness = 0;
	for (int row = 0; row < lowestRow; ++row) {
		bool lastHole = false;
		for (int column = 0; column < board.getNbrOfColumns(); ++column) {
			bool hole = board.getBlockType(row, column) == BlockType::EMPTY;
			if (lastHole != hole) {
				rowRoughness += 1;
				lastHole = hole;
			}
			if (!hole) {
				meanHeight += row;
				++nbrOfSquares;
			}
		}
	}

	meanHeight /= nbrOfSquares;

	double columnRoughness = 0;	
	for (int column = 0; column < board.getNbrOfColumns(); ++column) {
		bool lastHole = false;
		for (int row = 0; row < lowestRow; ++row) {
			bool hole = board.getBlockType(row, column) == BlockType::EMPTY;
			if (lastHole != hole) {
				columnRoughness += 1;
				lastHole = hole;
			}
		}
	}

	int edges = 0;
	int blockMeanHeight = 0;
	for (const Square& sq : block) {
		board.getBlockType(sq.row_, sq.column_-1) != BlockType::EMPTY ? ++edges : 0;
		board.getBlockType(sq.row_-1, sq.column_) != BlockType::EMPTY ? ++edges : 0;
		board.getBlockType(sq.row_, sq.column_+1) != BlockType::EMPTY ? ++edges : 0;
		blockMeanHeight += sq.row_;
	}
	blockMeanHeight /= 4;
	
	return ai_.calculateValue(rowRoughness,columnRoughness,edges,meanHeight,blockMeanHeight,board.getNbrOfRows(),board.getNbrOfColumns());
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
Computer::State Computer::calculateBestState(RawTetrisBoard board, int depth) {
	if (depth != 0) {
		std::vector<State> states = calculateAllPossibleStates(board, board.currentBlock());

		double highestValue = -100000;
		int hIndex = -1;
		for (unsigned int index = 0; index < states.size(); ++index) {
			State state = states[index];
			RawTetrisBoard childBoard = board;

			for (int i = 0; i < state.rotations_; ++i) {
				childBoard.update(Move::ROTATE_LEFT);
			}

			while (state.left_ != 0) {
				if (state.left_ < 0) {
					childBoard.update(Move::RIGHT);
					++state.left_;
				} else if (state.left_ > 0) {
					childBoard.update(Move::LEFT);
					--state.left_;
				}
			}

			// Move down the block and stop just before impact.
			for (int i = 0; i < state.down_-1; ++i) {
				childBoard.update(Move::DOWN_GRAVITY);
			}
			// Save the current block before impact.
			Block block = childBoard.currentBlock();
			// Impact, the block is now a part of the board.
			childBoard.update(Move::DOWN_GRAVITY);

			if (depth > 1) {
				calculateBestState(childBoard, depth - 1);
				double value = calculateValue(childBoard, block);

				if (value > highestValue) {
					hIndex = index;
					highestValue = value;
				}
			} else {
				double value = calculateValue(childBoard, block);

				if (value > highestValue) {
					hIndex = index;
					highestValue = value;
				}
			}
		}

		if (hIndex >= 0) {
			return states[hIndex];
		}
	}
	return State();
}

// Calculates and returns all posible states from the point of view from the block provided.
// The algortihm rotates and goes left or rigth with the block which then fall to the ground.
// I.e. The possible state.
std::vector<Computer::State> Computer::calculateAllPossibleStates(const RawTetrisBoard& board, Block block) const {
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
