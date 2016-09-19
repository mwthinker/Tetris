#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "device.h"

#include <SDL.h>
#include <SDL.h>

class Keyboard : public Device {
public:
	Keyboard(std::string name, SDL_Keycode down, SDL_Keycode left, SDL_Keycode right, SDL_Keycode rotate, SDL_Keycode downGround);

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
	SDL_Keycode down_, right_, left_, rotate_, downGround_;
	std::string name_;
	std::string playerName_;
};

#endif // KEYBOARD_H
