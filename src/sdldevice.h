#ifndef SDLDEVICE_H
#define SDLDEVICE_H

#include "device.h"

#include <SDL.h>

#include <memory>

class SdlDevice;
typedef std::shared_ptr<SdlDevice> SdlDevicePtr;

class SdlDevice : public IDevice {
public:
	virtual ~SdlDevice() = default;

	virtual void eventUpdate(const SDL_Event& windowEvent) {
	}

	void update(const TetrisBoard& board) override {
	}

	bool isAi() const override {
		return false;
	}
};

#endif // SDLDEVICE_H
