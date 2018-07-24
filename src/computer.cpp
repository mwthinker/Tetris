#include "computer.h"
#include "tetrisboard.h"

#include <vector>
#include <string>
#include <future>

Computer::Computer()
	: Device(true), currentTurn_(0), activeThread_(false) {
}

Computer::Computer(const Ai& ai)
	: Device(true), ai_(ai), currentTurn_(0), activeThread_(false) {
}

Input Computer::currentInput() {
	return input_;
}

std::string Computer::getName() const {
	return ai_.getName();
}

void Computer::update(const TetrisBoard& board) {
	// New block appears?
	if (currentTurn_ != board.getTurns() && !activeThread_) {
		activeThread_ = true;
		input_ = Input();
		currentTurn_ = board.getTurns();
		handle_ = std::async(std::launch::async | std::launch::deferred, Computer::calculateBestState, board, ai_, 1);
	} else {
		if (handle_.valid()) {
			latestState_ = handle_.get();
			latestBlock_ = board.getBlock();
			handle_ = std::future<Ai::State>();
			activeThread_ = false;
		}
		Block current = board.getBlock();
		Square currentSq = current.getRotationSquare();
		Square sq = latestBlock_.getRotationSquare();

		if (latestState_.left_ == sq.column_ - currentSq.column_) {
			latestState_.left_ = 0;
		}

		if (latestState_.rotationLeft_ == current.getCurrentRotation() - latestBlock_.getCurrentRotation()) {
			latestState_.rotationLeft_ = 0;
		}

		input_ = calculateInput(latestState_);
	}
}

Ai::State Computer::calculateBestState(RawTetrisBoard board, Ai ai, int depth) {
	return ai.calculateBestState(board, depth);
}

// Calculate and return the best input to achieve the current state.
Input Computer::calculateInput(Ai::State state) const {
	Input input;
	if (state.rotationLeft_ > 0) {
		input.rotate_ = !input_.rotate_;
	}
	if (state.left_ > 0) {
		input.left_ = true;
	} else if (state.left_ < 0) {
		input.right_ = true;
	}
	if (state.left_ == 0 && state.rotationLeft_ == 0) {
		input.down_ = true;
	}
	return input;
}
