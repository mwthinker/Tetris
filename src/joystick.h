#ifndef JOYSTICK_H
#define JOYSTICK_H

#include "device.h"

#include <mw/joystick.h>

class Joystick : public Device {
public:
	Joystick(mw::JoystickPtr joystick, int rotateButton = 0, int downButton = 1);
	~Joystick();

	Input currentInput() override;
	std::string getName() const override;

	std::string getPlayerName() const override {
		return playerName_;
	}

	void setPlayerName(std::string playerName) override {
		playerName_ = playerName;
	}

private:
	void eventUpdate(const SDL_Event& windowEvent) override;

	Input input_;
	mw::JoystickPtr joystick_;
	int rotateButton_, downButton_;
	std::string playerName_;
};

#endif // JOYSTICK_H
