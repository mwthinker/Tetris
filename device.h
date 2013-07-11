#ifndef DEVICE_H
#define DEVICE_H

#include <SDL.h>

#include <string>
#include <memory>

struct Input {
	Input() {
		rotate_ = false;
		down_ = false;
		left_ = false;
		right_ = false;
	}

	bool rotate_;
	bool down_;
	bool left_;
	bool right_;
};

class Device;
typedef std::shared_ptr<Device> DevicePtr;

class Device {
public:
	friend class TetrisWindow;

	virtual ~Device() {
	}

	virtual Input currentInput() = 0;

	virtual std::string getName() const = 0;

private:
    virtual void eventUpdate(const SDL_Event& windowEvent) = 0;
};

#endif // DEVICE_H
