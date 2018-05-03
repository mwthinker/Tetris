#include "ai.h"

#include <calc/cache.h>
#include <calc/calculatorexception.h>

#include <limits>
#include <cmath>
#include <iostream>

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

	float calculateValue(calc::Calculator& calculator, const calc::Cache& cache, const RawTetrisBoard& board, const Block& block) {
		int highestUsedRow = calculateHighestUsedRow(board);
		RowRoughness rowRoughness = calculateRowRoughness(board, highestUsedRow);
		ColumnRoughness columnRoughness = calculateColumnHoles(board, highestUsedRow);
		int edges = calculateBlockEdges(board, block);
		float blockMeanHeight = calculateBlockMeanHeight(block);

		calculator.updateVariable("rowHoles", (float) rowRoughness.holes_);
		calculator.updateVariable("columnHoles", (float) columnRoughness.holes_);
		calculator.updateVariable("bumpiness", (float) columnRoughness.bumpiness);
		calculator.updateVariable("edges", (float) edges);
		calculator.updateVariable("rowSumHeight", (float) rowRoughness.rowSum_);
		calculator.updateVariable("blockMeanHeight", blockMeanHeight);
		return calculator.excecute(cache);
	}

	inline int factorial(int number) {
		int result = number;
		while (number > 1) {
			number -= 1;
			result *= number;
		}
		return result;
	}

} // Anonymous namespace.

RowRoughness calculateRowRoughness(const RawTetrisBoard& board, int highestUsedRow) {
	RowRoughness rowRoughness;
	int holes = 0;
	for (int row = 0; row < highestUsedRow; ++row) {
		bool lastHole = board.getBlockType(0, row) == BlockType::EMPTY;
		for (int column = 0; column < board.getColumns(); ++column) {
			bool hole = board.getBlockType(column, row) == BlockType::EMPTY;
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
	return rowRoughness;
}


ColumnRoughness calculateColumnHoles(const RawTetrisBoard& board, int highestUsedRow) {
	ColumnRoughness roughness;
	int lastColumnNbr;
	for (int column = 0; column < board.getColumns(); ++column) {
		bool lastHole = board.getBlockType(column, 0) == BlockType::EMPTY;
		int columnNbr = lastHole ? 0 : 1;
		for (int row = 1; row < highestUsedRow; ++row) {
			bool hole = board.getBlockType(column, row) == BlockType::EMPTY;
			if (lastHole != hole) {
				roughness.holes_ += 1;
				lastHole = hole;
			}
			if (!hole) {
				++columnNbr;
			}
		}
		if (column != 0) {
			roughness.bumpiness += std::abs(lastColumnNbr - columnNbr);
		}
		lastColumnNbr = columnNbr;
	}
	return roughness;
}

// Calculation is perfomed before the block is part of the board.
int calculateLandingHeight(const Block& block) { // f1	
	return block.getLowestRow();
}

// Calculate the cleared lines times the current blocks square contribution to the cleared lines. 
// Calculation is perfomed before the block is part of the board.
int calculateErodedPieces(const RawTetrisBoard& board) { // f2	
	int w = board.getColumns();
	int h = board.getRows();
	const Block& block = board.getBlock();
	
	int clearedLines = 0;
	int contribution = 0;
	for (int y = 0; y < 4; ++y) {
		int row = block.getLowestRow() + y;
		int filled = board.calculateSquaresFilled(row);
		int rowContribution = 0;
		for (const Square& sq : block) {
			if (sq.row_ == row) {
				++rowContribution;
			}
		}
		if (filled + rowContribution == w) {
			// Line is full.
			clearedLines += 1;
			contribution += rowContribution;
		}
	}
	return clearedLines * contribution;
}

// Calculate the number of filled cells adjacent to empty cells summed of all rows.
int calculateRowTransitions(const RawTetrisBoard& board) { // f3
	const int w = board.getColumns();
	const int h = board.getRows();
	const int highestRow = calculateHighestUsedRow(board);

	int holes = 0;
	for (int y = 0; y <= highestRow; ++y) {
		bool lastFilled = true; // Left wall counts as filled.
		for (int x = 0; x < w; ++x) {
			bool filled = board.getBlockType(x, y) != BlockType::EMPTY;
			if (lastFilled != filled && filled) {
				++holes;
			}
			lastFilled = filled;
		}
		if (!lastFilled) {
			++holes;
		}
	}
	return holes;
}

// Calculate the number of filled cells adjacent to empty cells summed of all columns.
int calculateColumnTransitions(const RawTetrisBoard& board) { // f4
	const int w = board.getColumns();
	const int h = board.getRows();
	const int highestRow = calculateHighestUsedRow(board) + 1; // One higher, should be a empty square.

	int holes = 0;
	for (int x = 0; x < w; ++x) {
		bool lastFilled = true; // Upper limit counts as filled.
		for (int y = 0; y <= highestRow; ++y) {
			bool filled = board.getBlockType(x, y) != BlockType::EMPTY;
			if (lastFilled != filled && filled) {
				++holes;
			}
			lastFilled = filled;
		}
		if (!lastFilled) {
			++holes;
		}
	}
	return holes;
}

// Calculate the number of holes, the number of empty cells with at least one filled cell above.
int calculateNumberOfHoles(const RawTetrisBoard& board) { // f5
	int w = board.getColumns();
	int h = board.getRows();
	const int highestRow = calculateHighestUsedRow(board);

	int holes = 0;
	for (int x = 0; x < w; ++x) {
		bool foundFirstFilled = false; // Is always empty at the top. No hole.
		for (int y = highestRow; y >= 0; --y) {
			bool empty = board.getBlockType(x, y) == BlockType::EMPTY;
			if (!empty) {
				foundFirstFilled = true;
			}
			if (empty && foundFirstFilled) {
				++holes;
			}
		}
	}
	return holes;
}

// Calculate the sum of the accumulated depths of the wells.
int calculateCumulativeWells(const RawTetrisBoard& board) { // f6
	int w = board.getColumns();
	int h = board.getRows();
	const int highestRow = calculateHighestUsedRow(board);

	int cumulativeWells = 0;
	for (int x = 0; x < w; ++x) {
		int cumulative = 0;		
		for (int y = highestRow; y >= 0; --y) {
			bool empty = board.getBlockType(x, y) == BlockType::EMPTY;
			bool neighborsFilled = board.getBlockType(x - 1, y) != BlockType::EMPTY && board.getBlockType(x + 1, y) != BlockType::EMPTY;
			if (!empty) {
				break;
			}
			if (neighborsFilled) {
				++cumulative;
			}
			if (!neighborsFilled && cumulative > 0) {
				break;
			}
		}
		cumulativeWells += factorial(cumulative);
	}
	return cumulativeWells;
}

// Calculate the number of filled cells above holes summed over all columns.
int calculateHoleDepth(const RawTetrisBoard& board) { // f7
	return 0;
}

// Calculate the number of rows containing at least one hole.
int calculateRowHoles(const RawTetrisBoard& board) { // f7
	return 0;
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
	return (v.size() - index - 1) / board.getColumns();
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
		board.getBlockType(sq.column_ - 1, sq.row_) != BlockType::EMPTY ? ++edges : 0;
		board.getBlockType(sq.column_, sq.row_ - 1) != BlockType::EMPTY ? ++edges : 0;
		board.getBlockType(sq.column_ + 1, sq.row_) != BlockType::EMPTY ? ++edges : 0;
	}
	return edges;
}

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
	return calculateBestStateRecursive(board, depth, depth);
}

// Find the best state for the block to move.
Ai::State Ai::calculateBestStateRecursive(RawTetrisBoard board, int depth, int startDepth) {
	Ai::State bestState;

	if (depth != 0) {
		std::vector<Ai::State> states = calculateAllPossibleStates(board, board.getBlock());

		for (const Ai::State& state : states) {
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
				State childState = calculateBestStateRecursive(childBoard, depth - 1, startDepth);
				constexpr std::array<BlockType, 7> blockTypes = {BlockType::I, BlockType::J, BlockType::L,BlockType::O, BlockType::S, BlockType::T, BlockType::Z};

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

			/*
			if (startDepth - depth == 0) {  // 3-3=0, 3-2=1, 3-1=2, 3-0=3
				float value = calculateValue(calculator_, cache_, childBoard, block);
				if (value > bestState.value_) {
					bestState = state;
					bestState.value_ = value;
				}
			} else if (startDepth - depth == 1) {
				State childState = calculateBestStateRecursive(childBoard, depth - 1, startDepth);
				constexpr std::array<BlockType, 7> blockTypes = {BlockType::I, BlockType::J, BlockType::L,BlockType::O, BlockType::S, BlockType::T, BlockType::Z};

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
			}*/				
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
	calculator_.addVariable("bumpiness", 0);

	calculator_.addVariable("rows", 0);
	calculator_.addVariable("columns", 0);
	try {
		cache_ = calculator_.preCalculate(valueFunction_);
	} catch (calc::CalculatorException exception) {
		cache_ = calculator_.preCalculate("0");
	}
}
