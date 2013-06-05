#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "device.h"

#include <SDL.h>
#include <SDL.h>

class Keyboard : public Device {
public:
	Keyboard(std::string name, SDLKey down, SDLKey left, SDLKey right, SDLKey rotate);

	Input currentInput() override;
	std::string getName() const override;
private:
	void eventUpdate(const SDL_Event& windowEvent) override;

	Input input_;
	SDLKey down_, right_, left_, rotate_;
	std::string name_;
};

#endif // KEYBOARD_H
