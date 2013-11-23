#include "joystick.h"

/*
#include <mw/joystick.h>

#include <SDL.h>
#include <iostream>

Joystick::Joystick(mw::JoystickPtr joystick, int rotateButton, int downButton) : Device(false) {
	joystick_ = joystick;
	joystick->setActive(true);
	rotateButton_ = rotateButton;
	downButton_ = downButton;
}

Input Joystick::currentInput() {
	return input_;
}

std::string Joystick::getName() const {
	return joystick_->getName();
}

void Joystick::eventUpdate(const SDL_Event& windowEvent) {
	if (windowEvent.jaxis.which == joystick_->getJoystickIndex()) {
		switch (windowEvent.type) {
		case SDL_JOYAXISMOTION:  // Handle Joystick Motion
			if ( ( windowEvent.jaxis.value < -3200 ) || (windowEvent.jaxis.value > 3200 ) ) {
				if( windowEvent.jaxis.axis == 0) {
					std::cout << "\n LEFT/RIGHT";
				}
				if( windowEvent.jaxis.axis == 1) {
					std::cout << "\n UP/DOWN";
				}				
			}
			break;
		case SDL_JOYHATMOTION:
			if ( windowEvent.jhat.value & SDL_HAT_LEFT ) {
				//std::cout << "\n SDL_HAT_LEFT";
				input_.left_ = true;
			} else {
				input_.left_ = false;
			}

			if ( windowEvent.jhat.value & SDL_HAT_RIGHT) {
				input_.right_ = true;
			} else {
				input_.right_ = false;
			}
			if (windowEvent.jhat.value & SDL_HAT_DOWN) {
				input_.down_ = true;
			} else {
				input_.down_ = false;
			}
			break;
		case SDL_JOYBUTTONDOWN:  // Handle Joystick Button Presses
			if (windowEvent.jbutton.button == rotateButton_) {
				input_.rotate_ = true;
			}
			if (windowEvent.jbutton.button == downButton_) {
				input_.down_ = true;
			}
			break;
		case SDL_JOYBUTTONUP:  // Handle Joystick Button Presses
			if (windowEvent.jbutton.button == rotateButton_) {
				input_.rotate_ = false;
			}
			if (windowEvent.jbutton.button == downButton_) {
				input_.down_ = false;
			}
			break;
		default:
			break;
		}
	}
}
*/