#ifndef DEVICE_H
#define DEVICE_H

#include <SDL.h>

#include <memory>

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

typedef std::shared_ptr<Device> DevicePtr;

#endif // DEVICE_H
