#ifndef CONSOLEKEYBOARD_H
#define CONSOLEKEYBOARD_H

#include "device.h"

#include <console/console.h>

#include <string>

class ConsoleKeyboard : public IDevice {
public:
	ConsoleKeyboard(std::string name, 
		console::Key down, console::Key left, console::Key right,
		console::Key rotate, console::Key downGround);

	Input getInput() const override {
		return input_;
	}

	std::string getName() const override {
		return name_;
	}

	void eventUpdate(const console::ConsoleEvent& consoleEvent);

	void update(const TetrisBoard& board) override {
	}

	bool isAi() const override {
		return true;
	}

private:
	std::string name_, playerName_;	
	console::Key down_, right_, left_, rotate_, downGround_;

	Input input_;
};

#endif // CONSOLEKEYBOARD_H
