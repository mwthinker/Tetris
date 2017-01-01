#ifndef CONSOLEKEYBOARD_H
#define CONSOLEKEYBOARD_H

#include "device.h"

#include <string>

class ConsoleKeyboard : public Device {
public:
	ConsoleKeyboard(std::string name, char down, char left, char right, char rotate, char downGround);

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

	void eventUpdate(char key, double time);

private:
	class Key {
	public:
		Key(char key) : key_(key), pressedUntilTime_(0) {
		}

		bool isPressed(char key, double time) {
			if (key == key_) {
				pressedUntilTime_ = time + 0.02;
			}

			if (time < pressedUntilTime_) {
				return true;
			} else {
				return false;
			}
		}

	private:
		char key_;
		double pressedUntilTime_;
	};

	std::string name_, playerName_;
	Key down_, right_, left_, rotate_, downGround_;
	Input input_;
};

#endif // CONSOLEKEYBOARD_H
