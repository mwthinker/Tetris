#ifndef DEVICE_H
#define DEVICE_H

#include "input.h"

#include <SDL.h>

#include <string>
#include <memory>

class TetrisBoard;

class IDevice;
typedef std::shared_ptr<IDevice> IDevicePtr;

class IDevice {
public:
	virtual ~IDevice() = default;

	virtual Input getInput() const = 0;

	virtual std::string getName() const = 0;

	virtual void update(const TetrisBoard& board) = 0;

	virtual bool isAi() const = 0;
};

#endif // DEVICE_H
