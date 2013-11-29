#ifndef CUMPUTER_H
#define CUMPUTER_H

#include "device.h"
#include "block.h"
#include "rawtetrisboard.h"
#include "ai.h"

#include <vector>
#include <string>

class Computer : public Device {
public:
	Computer();

	Computer(const Ai& ai);

	Input currentInput() override;

	std::string getName() const override;

	void update(const TetrisBoard& board) override;

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

	double calculateValue(const RawTetrisBoard& board, const Block&) const;

	// Calculate and return the best input to achieve the current state.
	Input calculateInput(State state) const;

	// Find the best state for the block to move.
	State calculateBestState(RawTetrisBoard board, int depth);

	// Calculates and returns all posible states from the point of view from the block provided.
	// The algortihm rotates and goes left or rigth with the block which then fall to the ground.
	// I.e. The possible state.
	std::vector<State> calculateAllPossibleStates(const RawTetrisBoard& board, Block block) const;

	int latestId_;
	Input input_;
	State latestState_;
	Block latestBlock_;
	Ai ai_;
};

#endif // CUMPUTER_H