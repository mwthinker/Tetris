#ifndef DEVICE_H
#define DEVICE_H

#include <SDL.h>

struct Input {
	Input() {
		rotate = false;
		down = false;
		left = false;
		right = false;
	}

	bool rotate;
	bool down;
	bool left;
	bool right;
};

class Device {
public:
	virtual ~Device() {
	}

    virtual void eventUpdate(const SDL_Event& windowEvent) = 0;

	virtual Input currentInput() = 0;
};

#endif // DEVICE_H

