#include "computer.h"
#include "tetrisboard.h"

#include <vector>
#include <string>
#include <future>
#include <iostream>

namespace {

	// All parameters are copied, to avoid thread problems.
	Ai::State asyncCalculateBestState(RawTetrisBoard board, Ai ai, int depth) {
		return ai.calculateBestState(board, depth);
	}

}

Computer::Computer() : Device(true) {
	nbrOfUpdates_ = -1;
	activeThread_ = false;
}

Computer::Computer(const Ai& ai) : Device(true) {
	nbrOfUpdates_ = -1;
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
	if (nbrOfUpdates_ != board.getNbrOfUpdates() && !activeThread_) {
		activeThread_ = true;
		input_ = Input();
		nbrOfUpdates_ = board.getNbrOfUpdates();
		handle_ = std::async(std::launch::async | std::launch::deferred, asyncCalculateBestState, board, ai_, 1);
	} else {
		if (handle_.valid()) {
			latestState_ = handle_.get();
			latestBlock_ = board.getBlock();
			input_ = calculateInput(latestState_);
			handle_ = std::future<Ai::State>();
			activeThread_ = false;
		}
		Block current = board.getBlock();
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

// Calculate and return the best input to achieve the current state.
Input Computer::calculateInput(Ai::State state) const {
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
