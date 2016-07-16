#include "ai.h"

namespace {

	// Calculates and returns all possible states from the point of view from the block provided.
	// The algorithm rotates and goes left or right with the block which then fall to the ground.
	// I.e. The possible state.
	std::vector<Ai::State> calculateAllPossibleStates(const RawTetrisBoard& board, Block block, int removeRows) {
		std::vector<Ai::State> states;
		// Valid block position?
		if (!board.collision(block)) {
			Ai::State state;

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
					states.push_back(Ai::State(stepsDown, stepsLeft, rotations));

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
					states.push_back(Ai::State(stepsDown, stepsLeft, rotations));

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

	float calculateValue(RawTetrisBoard& board, const Block& block, int removeRows) {
		int lowestRow = board.getRows();
		int columns = board.getColumns();
		float meanHeight = 0;
		int nbrOfSquares = 0;

		// Aggregate Height and bumpiness.
		float aggregateHeight = 0.0;
		float bumpiness = 0.0;
		for (int column = 0; column < columns; ++column) {
			int lastHeight = 0;
			for (int row = lowestRow - 1; row >= 0; --row) {
				bool hole = board.getBlockType(row, column) == BlockType::EMPTY;
				if (board.getBlockType(row, column) != BlockType::EMPTY) {
					aggregateHeight += row + 1;
					if (column > 0) { // Ignore first column.
						bumpiness += std::abs(lastHeight - aggregateHeight);
					}
					lastHeight = aggregateHeight;
					continue;
				}
			}
		}

		// Complete lines.
		float completeLines = removeRows;
		if (completeLines > 0) {
			//std::cout << "\n------------------------ " << completeLines << " ------------------------------------\n";

		}
		/*for (int row = lowestRow - 1; row >= 0; --row) {
		int nbr = 0;
		for (int column = 0; column < columns; ++column) {
		if (board.getBlockType(row, column) != BlockType::EMPTY) {
		++nbr;
		if (nbr == columns) {
		completeLines += 1;
		}
		continue;
		}
		}
		}*/

		// Holes.
		float holes = 0;
		for (int column = 0; column < columns && completeLines < 4; ++column) {
			bool upperSquare = false;
			for (int row = lowestRow - 1; row >= 0; --row) {
				if (board.getBlockType(row, column) != BlockType::EMPTY) {
					upperSquare = true;
				} else if (upperSquare) {
					++holes;
				}
			}
		}

		//return -100 * aggregateHeight + 0.760666f * completeLines - 10 * holes - 1 * bumpiness;
		return -0.510066f * aggregateHeight + 0.760666f * completeLines - 0.35663f * holes - 0.184483f * bumpiness;
	}

	

	/*
	// Find the best state for the block to move.
	Ai::State calculateBestState(RawTetrisBoard board, int depth, int removeRows) {
		if (depth != 0) {
			std::vector<Ai::State> states = calculateAllPossibleStates(board, board.getBlock(), removeRows);

			float highestValue = -100000;
			int hIndex = -1;
			for (unsigned int index = 0; index < states.size(); ++index) {
				Ai::State state = states[index];
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
				for (int i = 0; i < state.down_ - 1; ++i) {
					childBoard.update(Move::DOWN_GRAVITY);
				}
				// Save the current block before impact.
				Block block = childBoard.getBlock();
				// Impact, the block is now a part of the board.
				childBoard.update(Move::DOWN_GRAVITY);

				if (depth > 1) {
					calculateBestState(childBoard, depth - 1, childBoard.getRemovedRows() - board.getRemovedRows() + removeRows);
					float value = calculateValue(childBoard, block, childBoard.getRemovedRows() - board.getRemovedRows() + removeRows);

					if (value > highestValue) {
						hIndex = index;
						highestValue = value;
					}
				} else {
					float value = calculateValue(childBoard, block, childBoard.getRemovedRows() - board.getRemovedRows() + removeRows);

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
		return Ai::State();
	}
	*/

} // Anonymous namespace.

Ai::State Ai::calculateBestState(RawTetrisBoard board, int depth) {
	return calculateBestState(board, depth, 0);
}

// Find the best state for the block to move.
Ai::State Ai::calculateBestState(RawTetrisBoard board, int depth, int removeRows) {
	if (depth != 0) {
		std::vector<Ai::State> states = calculateAllPossibleStates(board, board.getBlock(), removeRows);

		double highestValue = -100000;
		int hIndex = -1;
		for (unsigned int index = 0; index < states.size(); ++index) {
			Ai::State state = states[index];
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
			for (int i = 0; i < state.down_ - 1; ++i) {
				childBoard.update(Move::DOWN_GRAVITY);
			}
			// Save the current block before impact.
			Block block = childBoard.getBlock();
			// Impact, the block is now a part of the board.
			childBoard.update(Move::DOWN_GRAVITY);

			if (depth > 1) {
				calculateBestState(childBoard, depth - 1, removeRows);
				double value = calculateValue(calculator_, cache_, childBoard, block);

				if (value > highestValue) {
					hIndex = index;
					highestValue = value;
				}
			} else {
				double value = calculateValue(calculator_, cache_, childBoard, block);

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
	return Ai::State();
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
