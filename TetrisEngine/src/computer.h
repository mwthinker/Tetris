#ifndef CUMPUTER_H
#define CUMPUTER_H

#include "device.h"
#include "block.h"
#include "rawtetrisboard.h"
#include "ai.h"

#include <calc/calculator.h>

#include <vector>
#include <string>
#include <future>

class Computer : public Device {
public:
	Computer();

	Computer(const Ai& ai);

	Input currentInput() override;

	std::string getName() const override;

	std::string getPlayerName() const override;

	void setPlayerName(std::string playerName) override {
		playerName_ = playerName;
	}

	void update(const TetrisBoard& board) override;

	struct State {
		State() : down_(0), left_(0), rotations_(0) {
		}

		State(int down, int left, int rotations) : down_(down), left_(left), rotations_(rotations) {
		}

		int down_;
		int left_;
		int rotations_;
	};

	inline calc::Calculator& getCalculator() {
		return calculator_;
	}

	inline const calc::Cache& getCache() {
		return cache_;
	}

private:
	void initCalculator(const Ai& ai);

	// Calculate and return the best input to achieve the current state.
	Input calculateInput(State state) const;

	int nbrOfUpdates_;
	Input input_;
	State latestState_;
	Block latestBlock_;
	Ai ai_;
	bool activeThread_;
	std::future<State> handle_;
	std::string playerName_;
	calc::Calculator calculator_;
	calc::Cache cache_;
};

Computer::State calculateBestState(calc::Calculator& calculator, const calc::Cache& cache, RawTetrisBoard board, int depth);

#endif // CUMPUTER_H
