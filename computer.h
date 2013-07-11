#ifndef CUMPUTER_H
#define CUMPUTER_H

#include "device.h"
#include "tetrisboard.h"

#include <vector>

class Computer {
public:
	Computer(const TetrisBoard& board) : board_(board) {
	}

	// Find all possible positions for the block.
	void update(const Block& current) {
		int row = findLowesEmptyRow(board_);
		int diff = current.getLowestRow() - row;
		if (diff >= 0) {
			Block newBlock = current;
			for (int i = 0; i < diff; ++i) {
				newBlock.moveDown();
			}

			getAllPossiblePositions(newBlock);
		}
	}

private:
	struct State {
		State(int down, int left, int rotations) : down_(down), left_(left), rotations_(rotations) {
		}

		int down_;
		int left_;
		int rotations_;
	};

	typedef  std::vector<State> Path;

	std::vector<State>& getAllPossiblePositions(Block& block) {
		State state(0,0,0);
		Path path(1, state);
		std::vector<Path> paths;
		calculateAllPossiblePositions(block, paths, path);
	}

	// Saves all (path) to valid position in (paths) and block is the current block.
	void calculateAllPossiblePositions(Block block, std::vector<Path>& paths, const Path& currentPath) {
		State state = currentPath.back();
		block.moveDown();
		++state.down_;
		if (!board_.collision(block)) {
			paths.push_back(currentPath);

			// Go through all rotations for the block.
			for (; state.rotations_ < block.getNumberOfRotations(); ++state.rotations_, block.rotateLeft()) {
				// Go left.
				Path lPath = currentPath;
				Block tmp = block;
				tmp.moveLeft();
				State lState = state;
				while (!board_.collision(tmp)) {
					++lState.left_;
					Path lPath = currentPath;
					lPath.push_back(state);
					calculateAllPossiblePositions(block, paths, lPath);
					tmp.moveLeft();
				}

				// Go right.
				Path rPath = currentPath;
				tmp = block;
				tmp.moveRight();
				State rState = state;
				while (!board_.collision(tmp)) {
					++rState.left_;
					Path rPath = currentPath;
					rPath.push_back(state);
					calculateAllPossiblePositions(block, paths, rPath);
					tmp.moveRight();
				}
			}
		} else {
			paths.push_back(currentPath);
		}
	}

	int findLowesEmptyRow(const TetrisBoard& board) const {
		int maxRow = board_.getNbrOfRows();
		for (int row = 0; row < maxRow; ++row) {
			if (board_.nbrOfSquares(row) == 0) {
				return row;
			}
		}
		return maxRow;
	}

	const TetrisBoard& board_;
};

#endif // CUMPUTER_H
