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
		return Input();
	}

	std::string getName() const override {
		return "Computer";
	}

	void update(const TetrisBoard& board) override {
		//updateD(board);
	}

private:
	// Find all possible positions for the block.
	void updateD(const TetrisBoard& board) {
		int row = findLowestEmptyRow(board);
		Block current = board.currentBlock();
		int diff = current.getLowestRow() - row;
		
		if (diff >= 0) {
			Block newBlock = current;
			for (int i = 0; i < diff; ++i) {
				newBlock.moveDown();
			}
			getAllPossiblePositions(board, current);
		}
	}

	struct State {
		State(int down, int left, int rotations) : down_(down), left_(left), rotations_(rotations) {
		}

		int down_;
		int left_;
		int rotations_;
	};

	typedef  std::vector<State> Path;

	std::vector<State> getAllPossiblePositions(const TetrisBoard& board, const Block& block) {
		State state(0,0,0);
		Path path(1, state);
		std::vector<Path> paths;
		calculateAllPossiblePositions(board, block, paths, path);

		return std::vector<State>();
	}

	// Saves all (path) to valid position in (paths) and block is the current block.
	void calculateAllPossiblePositions(const TetrisBoard& board, Block block, std::vector<Path>& paths, const Path& currentPath) {
		State state = currentPath.back();
		block.moveDown();
		++state.down_;
		if (!board.collision(block)) {
			paths.push_back(currentPath);

			// Go through all rotations for the block.
			for (; state.rotations_ <= block.getNumberOfRotations(); ++state.rotations_, block.rotateLeft()) {
				// Go left.
				Path lPath = currentPath;
				Block tmp = block;
				tmp.moveLeft();
				State lState = state;
				while (!board.collision(tmp)) {
					++lState.left_;
					//Path lPath = currentPath;
					lPath.push_back(state);
					calculateAllPossiblePositions(board, tmp, paths, lPath);
					tmp.moveLeft();
				}

				// Go right.
				Path rPath = currentPath;
				tmp = block;
				tmp.moveRight();
				State rState = state;
				while (!board.collision(tmp)) {
					++rState.left_;
					Path rPath = currentPath;
					//rPath.push_back(state);
					calculateAllPossiblePositions(board, tmp, paths, rPath);
					tmp.moveRight();
				}
			}
		} else {
			paths.push_back(currentPath);
		}
	}

	int findLowestEmptyRow(const TetrisBoard& board) const {
		int maxRow = board.getNbrOfRows();
		for (int row = 0; row < maxRow; ++row) {
			if (board.nbrOfSquares(row) == 0) {
				return row;
			}
		}
		return maxRow;
	}	
};

#endif // CUMPUTER_H
