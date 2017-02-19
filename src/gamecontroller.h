#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "sdldevice.h"

#include <mw/gamecontroller.h>

class GameController : public SdlDevice {
public:
	GameController(mw::GameControllerPtr gameController, int rotateButton = 0, int downButton = 1);

	Input currentInput() override;
	std::string getName() const override;

	std::string getPlayerName() const override {
		return playerName_;
	}

	void setPlayerName(std::string playerName) override {
		playerName_ = playerName;
	}

	inline int getInstanceId() const {
		return gameController_->getInstanceId();
	}

private:
	void eventUpdate(const SDL_Event& windowEvent) override;

	void updateInput(Uint8 button, bool state);

	Input input_;
	mw::GameControllerPtr gameController_;
	int rotateButton_, downButton_;
	std::string playerName_;
};

#endif // GAMECONTROLLER_H
