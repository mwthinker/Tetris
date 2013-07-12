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

class TetrisBoard;

class Device;
typedef std::shared_ptr<Device> DevicePtr;

class Device {
public:
	virtual ~Device() {
	}

	virtual Input currentInput() = 0;

	virtual std::string getName() const = 0;

	virtual void update(const TetrisBoard& board) {
	}

	virtual void eventUpdate(const SDL_Event& windowEvent) {
	}
};

#endif // DEVICE_H
