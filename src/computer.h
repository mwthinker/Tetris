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

private:
	// Calculate and return the best input to achieve the current state.
	Input calculateInput(Ai::State state) const;

	int nbrOfUpdates_;
	Input input_;
	Ai::State latestState_;
	Block latestBlock_;
	Ai ai_;
	bool activeThread_;
	std::future<Ai::State> handle_;
	std::string playerName_;
};

#endif // CUMPUTER_H
