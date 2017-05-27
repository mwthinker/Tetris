#ifndef CONSOLEKEYBOARD_H
#define CONSOLEKEYBOARD_H

#include "device.h"
#include "console.h"

#include <string>

class ConsoleKeyboard : public Device {
public:
	ConsoleKeyboard(std::string name, console::Key down, console::Key left, console::Key right, console::Key rotate, console::Key downGround);

	inline Input currentInput() override {
		return input_;
	}

	inline std::string getName() const override {
		return name_;
	}

	inline std::string getPlayerName() const override {
		return playerName_;
	}

	inline void setPlayerName(std::string playerName) override {
		playerName_ = playerName;
	}

	void eventUpdate(const console::ConsoleEvent& consoleEvent);

private:
	std::string name_, playerName_;	
	console::Key down_, right_, left_, rotate_, downGround_;

	Input input_;
};

#endif // CONSOLEKEYBOARD_H
