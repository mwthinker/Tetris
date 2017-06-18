#include "ai.h"

namespace {

	// Calculate and return all possible states from the point of view from the block provided.
	std::vector<Ai::State> calculateAllPossibleStates(const RawTetrisBoard& board, Block block, int removeRows) {
		std::vector<Ai::State> states;
		
		// Valid block position?
		if (!board.collision(block)) {
			// Go through all rotations for the block.
			for (int rotationLeft = 0; rotationLeft <= block.getNumberOfRotations(); ++rotationLeft, block.rotateLeft()) {
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
					states.emplace_back(stepsLeft, rotationLeft);

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
					states.emplace_back(stepsLeft, rotationLeft);

					--stepsLeft;
					horizontal.moveRight();
				}
			}
		}
		return states;
	}

	float calculateValue(calc::Calculator& calculator, const calc::Cache& cache, RawTetrisBoard& board, const Block& block) {
		int lowestRow = board.getRows();
		float meanHeight = 0;
		int nbrOfSquares = 0;

		float rowRoughness = 0;
		for (int row = 0; row < lowestRow; ++row) {
			bool lastHole = false;
			for (int column = 0; column < board.getColumns(); ++column) {
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

		float columnRoughness = 0;
		for (int column = 0; column < board.getColumns(); ++column) {
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
		float blockMeanHeight = 0;
		for (const Square& sq : block) {
			board.getBlockType(sq.row_, sq.column_ - 1) != BlockType::EMPTY ? ++edges : 0;
			board.getBlockType(sq.row_ - 1, sq.column_) != BlockType::EMPTY ? ++edges : 0;
			board.getBlockType(sq.row_, sq.column_ + 1) != BlockType::EMPTY ? ++edges : 0;
			blockMeanHeight += sq.row_;
		}
		blockMeanHeight /= 4;

		calculator.updateVariable("rowRoughness", rowRoughness);
		calculator.updateVariable("columnRoughness", columnRoughness);
		calculator.updateVariable("edges", (float) edges);
		calculator.updateVariable("meanHeight", meanHeight);
		calculator.updateVariable("blockMeanHeight", blockMeanHeight);
		calculator.updateVariable("rows", (float) board.getRows());
		calculator.updateVariable("columns", (float) board.getColumns());

		return calculator.excecute(cache);
	}

} // Anonymous namespace.

Ai::State Ai::calculateBestState(RawTetrisBoard board, int depth) {
	return calculateBestState(board, depth, 0);
}

// Find the best state for the block to move.
Ai::State Ai::calculateBestState(RawTetrisBoard board, int depth, int removeRows) {
	Ai::State bestState;

	if (depth != 0) {
		std::vector<Ai::State> states = calculateAllPossibleStates(board, board.getBlock(), removeRows);

		for (const Ai::State state : states) {
			RawTetrisBoard childBoard = board;

			// Rotate.
			for (int i = 0; i < state.rotationLeft_; ++i) {
				childBoard.update(Move::ROTATE_LEFT);
			}
			
			// Move left if is is possible.
			for (int i = 0; i < state.left_; ++i) {
				childBoard.update(Move::LEFT);
			}
			// Move right if is is possible.
			for (int i = 0; i < -1 * state.left_; ++i) {
				childBoard.update(Move::RIGHT);
			}

			// Move down the block and stop just before impact.
			childBoard.update(Move::DOWN_GROUND);
			// Save the current block before impact.
			Block block = childBoard.getBlock();
			// Impact, the block is now a part of the board.
			childBoard.update(Move::DOWN_GRAVITY);

			if (depth > 1) {
				State childState = calculateBestState(childBoard, depth - 1, removeRows);

				if (childState.value_ > bestState.value_) {
					bestState = state;
					// Only updating the value from the child.
					bestState.value_ = childState.value_;
				}
			} else {
				float value = calculateValue(calculator_, cache_, childBoard, block);

				if (value > bestState.value_) {
					bestState = state;
					bestState.value_ = value;
				}
			}
		}
	}
	return bestState;
}

void Ai::initCalculator() {
	calculator_.addVariable("rowRoughness", 0);
	calculator_.addVariable("columnRoughness", 0);
	calculator_.addVariable("edges", 0);
	calculator_.addVariable("meanHeight", 0);
	calculator_.addVariable("blockMeanHeight", 0);
	calculator_.addVariable("rows", 0);
	calculator_.addVariable("columns", 0);

	cache_ = calculator_.preCalculate(valueFunction_);
}
