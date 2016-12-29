#ifndef SDLDEVICE_H
#define SDLDEVICE_H

#include "device.h"

#include <SDL.h>

#include <memory>

class SdlDevice;
typedef std::shared_ptr<SdlDevice> SdlDevicePtr;

class SdlDevice : public Device {
public:
	inline SdlDevice(bool ai) : Device(ai) {
	}

	inline virtual ~SdlDevice() {
	}

	inline virtual void eventUpdate(const SDL_Event& windowEvent) {
	}
};

#endif // SDLDEVICE_H
