#include "ai.h"

#include <limits>

#include <calc/cache.h>

namespace {

	// Calculate and return all possible states for the block provided.
	std::vector<Ai::State> calculateAllPossibleStates(const RawTetrisBoard& board, Block block) {
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

	struct RowRoughness {
		RowRoughness() : holes_(0), rowSum_(0) {
		}

		int holes_;
		int rowSum_;
	};

	RowRoughness calculateRowRoughness(const RawTetrisBoard& board, int highestUsedRow) {
		RowRoughness rowRoughness;
		int holes = 0;
		for (int row = 0; row < highestUsedRow; ++row) {
			bool lastHole = board.getBlockType(row, 0) == BlockType::EMPTY;
			for (int column = 1; column < board.getColumns(); ++column) {
				bool hole = board.getBlockType(row, column) == BlockType::EMPTY;
				if (lastHole != hole) {
					rowRoughness.holes_ += 1;
					lastHole = hole;
				}
				if (!hole) {
					rowRoughness.rowSum_ += row;
					++holes;
				}
			}
		}
		//rowRoughness.meanHeight_ /= holes;
		//rowRoughness.meanHeight_ /= holes;
		return rowRoughness;
	}

	int calculateColumnHoles(const RawTetrisBoard& board, int highestUsedRow) {
		int columnHoles = 0;
		for (int column = 0; column < board.getColumns(); ++column) {
			bool lastHole = board.getBlockType(0, column) == BlockType::EMPTY;
			for (int row = 1; row < highestUsedRow; ++row) {
				bool hole = board.getBlockType(row, column) == BlockType::EMPTY;
				if (lastHole != hole) {
					columnHoles += 1;
					lastHole = hole;
				}
			}
		}
		return columnHoles;
	}

	int calculateHighestUsedRow(const RawTetrisBoard& board) {
		auto v = board.getBoardVector();
		int index = 0;
		for (auto it = v.rbegin(); it != v.rend(); ++it) {
			if (*it != BlockType::EMPTY) {
				index = it - v.rbegin();
				break;
			}
		}
		return (v.size() - index - 1) / board.getColumns() + 2;
	}

	float calculateBlockMeanHeight(const Block& block) {
		int blockMeanHeight = 0;
		for (const Square& sq : block) {
			blockMeanHeight += sq.row_;
		}
		return (float) blockMeanHeight / block.getSize();
	}

	int calculateBlockEdges(const RawTetrisBoard& board, const Block& block) {
		int edges = 0;
		for (const Square& sq : block) {
			board.getBlockType(sq.row_, sq.column_ - 1) != BlockType::EMPTY ? ++edges : 0;
			board.getBlockType(sq.row_ - 1, sq.column_) != BlockType::EMPTY ? ++edges : 0;
			board.getBlockType(sq.row_, sq.column_ + 1) != BlockType::EMPTY ? ++edges : 0;
		}
		return edges;
	}

	float calculateValue(calc::Calculator& calculator, const calc::Cache& cache, const RawTetrisBoard& board, const Block& block) {
		int highestUsedRow = calculateHighestUsedRow(board);
		
		RowRoughness rowRoughness = calculateRowRoughness(board, highestUsedRow);
		int columnHoles = calculateColumnHoles(board, highestUsedRow);
		int edges = calculateBlockEdges(board, block);
		float blockMeanHeight = calculateBlockMeanHeight(block);
		
		calculator.updateVariable("rowHoles", (float) rowRoughness.holes_);
		calculator.updateVariable("columnHoles", (float) columnHoles);
		calculator.updateVariable("edges", (float) edges);
		calculator.updateVariable("rowSumHeight", (float) rowRoughness.rowSum_);
		calculator.updateVariable("blockMeanHeight", blockMeanHeight);

		return calculator.excecute(cache);
	}

} // Anonymous namespace.

Ai::State::State() : left_(0), rotationLeft_(0), value_(std::numeric_limits<float>::lowest()) {
}

Ai::State::State(int left, int rotations) : left_(left), rotationLeft_(rotations), value_(std::numeric_limits<float>::lowest()) {
}

Ai::Ai() : Ai("DefaultAi", "-2*rowHoles - 5*columnHoles - 1*rowSumHeight / (1 + rowHoles) - 2*blockMeanHeight") {
}

Ai::Ai(std::string name, std::string valueFunction) : name_(name), valueFunction_(valueFunction) {
	initCalculator();
}

Ai::State Ai::calculateBestState(RawTetrisBoard board, int depth) {
	calculator_.updateVariable("rows", (float) board.getRows());
	calculator_.updateVariable("columns", (float) board.getColumns());
	return calculateBestStateRecursive(board, depth);
}

// Find the best state for the block to move.
Ai::State Ai::calculateBestStateRecursive(RawTetrisBoard board, int depth) {
	Ai::State bestState;

	if (depth != 0) {
		std::vector<Ai::State> states = calculateAllPossibleStates(board, board.getBlock());

		for (const Ai::State state : states) {
			RawTetrisBoard childBoard = board;

			// Rotate.
			for (int i = 0; i < state.rotationLeft_; ++i) {
				childBoard.update(Move::ROTATE_LEFT);
			}
			
			// Move left.
			for (int i = 0; i < state.left_; ++i) {
				childBoard.update(Move::LEFT);
			}
			// Move right.
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
				State childState = calculateBestState(childBoard, depth - 1);

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
	calculator_.addVariable("rowHoles", 0);
	calculator_.addVariable("columnHoles", 0);
	calculator_.addVariable("edges", 0);
	calculator_.addVariable("rowSumHeight", 0);
	calculator_.addVariable("blockMeanHeight", 0);
	calculator_.addVariable("rows", 0);
	calculator_.addVariable("columns", 0);

	cache_ = calculator_.preCalculate(valueFunction_);
}
