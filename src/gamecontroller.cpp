#include "gamecontroller.h"

#include <mw/gamecontroller.h>

#include <SDL.h>

GameController::GameController(const mw::GameControllerPtr& gameController, int rotateButton, int downButton) {
	gameController_ = gameController;
	rotateButton_ = rotateButton;
	downButton_ = downButton;
}

Input GameController::getInput() const {
	return input_;
}

std::string GameController::getName() const {
	return gameController_->getName();
}

void GameController::eventUpdate(const SDL_Event& windowEvent) {
	switch (windowEvent.type) {
		case SDL_CONTROLLERBUTTONDOWN:
			if (gameController_->isAttached() && windowEvent.cbutton.which == gameController_->getInstanceId()) {
				updateInput(windowEvent.cbutton.button, true);
			}
			break;
		case SDL_CONTROLLERBUTTONUP:
			if (gameController_->isAttached() && windowEvent.cbutton.which == gameController_->getInstanceId()) {
				updateInput(windowEvent.cbutton.button, false);
			}
			break;
		default:
			break;
	}
}

void GameController::updateInput(Uint8 button, bool state) {
	switch (button) {
		case SDL_CONTROLLER_BUTTON_A:
			input_.downGround_ = state;
			break;
		case SDL_CONTROLLER_BUTTON_X:
			input_.rotate_ = state;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			input_.rotate_ = state;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			input_.down_ = state;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			input_.left_ = state;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			input_.right_ = state;
			break;
		default:
			break;
	}
}
