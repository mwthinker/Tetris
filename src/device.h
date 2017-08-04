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
		downGround_ = false;
	}

	bool rotate_;
	bool down_;
	bool downGround_;
	bool left_;
	bool right_;
};

class TetrisBoard;

class Device;
typedef std::shared_ptr<Device> DevicePtr;

class Device {
public:
	Device(bool ai) : ai_(ai) {
	}

	virtual ~Device() {
	}

	virtual Input currentInput() = 0;

	virtual std::string getName() const = 0;

	virtual void update(const TetrisBoard& board) {
	}

	bool isAi() const {
		return ai_;
	}

private:
	const bool ai_;
};

#endif // DEVICE_H
