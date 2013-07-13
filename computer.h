#ifndef CUMPUTER_H
#define CUMPUTER_H

#include "device.h"
#include "tetrisboard.h"

#include <vector>
#include <string>

class Computer : public Device {
public:
	Computer() {
	}

	Input currentInput() override {
		return input_;
	}

	std::string getName() const override {
		return "Computer";
	}

	void update(const TetrisBoard& board) override {
		State state = calculateBestState(board);
		input_ = calculateInput(state);
	}

private:
	struct State {
		State() : down_(0), left_(0), rotations_(0) {
		}

		State(int down, int left, int rotations) : down_(down), left_(left), rotations_(rotations) {
		}

		int down_;
		int left_;
		int rotations_;
	};

	int calculateValue(const TetrisBoard& board) const {
		int value = 0;
		const int MAXROWS = board.getNbrOfRows();
		for (int row = 0; row < MAXROWS; ++row) {
			int nbr = board.nbrOfSquares(row); 
			value -= nbr * row;
		}
		return value;
	}

	// Calulate and return the best input to ashieve the current state.
	Input calculateInput(State state) const {
		Input input;
		if (state.rotations_ > 0) {
			input.rotate_ = true;
		}
		if (state.left_ > 0) {
			input.left_ = true;
		} else if (state.left_ < 0) {
			input.right_ = true;
		}
		if (state.down_ != 0 && state.left_ == 0 && state.rotations_ == 0) {
			input.down_ = true;
		}
		return input;
	}

	// Find the best state for the block to move.
	State calculateBestState(const TetrisBoard& board) {
		std::vector<State> states = calculateAllPossibleStates(board, board.currentBlock());

		int highestValue = -100000;
		int hIndex = -1;
		for (unsigned int index = 0; index < states.size(); ++index) {
			State state = states[index];
			TetrisBoard childBoard = board;
			childBoard.add(BlockType::BLOCK_TYPE_EMPTY);
				
			for (int i = 0; i < state.rotations_; ++i) {
				childBoard.update(TetrisBoard::ROTATE_LEFT);
			}
				
			while (state.left_ != 0) {
				if (state.left_ < 0) {
					childBoard.update(TetrisBoard::RIGHT);
					++state.left_;
				} else if (state.left_ > 0) {
					childBoard.update(TetrisBoard::LEFT);
					--state.left_;
				}
			}

			for (int i = 0; i <= state.down_; ++i) {
				childBoard.update(TetrisBoard::DOWN_GRAVITY);
			}

			int value = calculateValue(childBoard);
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
	std::vector<State> calculateAllPossibleStates(const TetrisBoard& board, Block block) const {
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
	
	// Returns the lowest empty row on the board.
	int findLowestEmptyRow(const TetrisBoard& board) const {
		int maxRow = board.getNbrOfRows();
		for (int row = 0; row < maxRow; ++row) {
			if (board.nbrOfSquares(row) == 0) {
				return row;
			}
		}
		return maxRow;
	}

	Input input_;
};

#endif // CUMPUTER_H
