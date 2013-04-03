#ifndef INPUTJOYSTICK_H
#define INPUTJOYSTICK_H

#include "inputdevice.h"
#include "human.h"

#include <mw/joystick.h>

#include <SDL.h>
#include <iostream>

class InputJoystick : public InputDevice<PlayerEvent> {
public:
	InputJoystick(mw::JoystickPtr joystick, int rotateButton = 0, int downButton = 1) {
		joystick_ = joystick;
		joystick->setActive(true);
		
		rotateButton_ = rotateButton;
		downButton_ = downButton;
	}

	void eventUpdate(const SDL_Event& windowEvent) {
		if (windowEvent.jaxis.which == joystick_->getJoystickIndex()) {
			//std::cout << windowEvent.jaxis.which+1 << "\n";
			switch (windowEvent.type) {
			case SDL_JOYAXISMOTION:  // Handle Joystick Motion
				if ( ( windowEvent.jaxis.value < -3200 ) || (windowEvent.jaxis.value > 3200 ) ) {
					if( windowEvent.jaxis.axis == 0) 
					{
						std::cout << "\n LEFT/RIGHT";
					}

					if( windowEvent.jaxis.axis == 1) 
					{
						std::cout << "\n UP/DOWN";
					}				
				}
				break;
			case SDL_JOYHATMOTION:
				if ( windowEvent.jhat.value & SDL_HAT_LEFT ) {
					//std::cout << "\n SDL_HAT_LEFT";
					InputDevice<PlayerEvent>::pushEvent(PLAYER_LEFT);
				} else {
					InputDevice<PlayerEvent>::pushEvent(PLAYER_UN_LEFT);
				}

				if ( windowEvent.jhat.value & SDL_HAT_RIGHT)
				{
					//std::cout << "\n SDL_HAT_RIGHT";
					InputDevice<PlayerEvent>::pushEvent(PLAYER_RIGHT);
				} else {
					InputDevice<PlayerEvent>::pushEvent(PLAYER_UN_RIGHT);
				}

				if (windowEvent.jhat.value & SDL_HAT_DOWN)
				{
					//std::cout << "\n SDL_HAT_RIGHT";
					InputDevice<PlayerEvent>::pushEvent(PLAYER_DOWN);
				} else {
					InputDevice<PlayerEvent>::pushEvent(PLAYER_UN_DOWN);
				}
				break;
			case SDL_JOYBUTTONDOWN:  // Handle Joystick Button Presses
				if (windowEvent.jbutton.button == rotateButton_) 
				{
					//std::cout << "\n BUTTON " << (int) windowEvent.jbutton.button;
					InputDevice<PlayerEvent>::pushEvent(PLAYER_ROTATE);
				}
				if (windowEvent.jbutton.button == downButton_) 
				{
					//std::cout << "\n BUTTON " << (int) windowEvent.jbutton.button;
					InputDevice<PlayerEvent>::pushEvent(PLAYER_DOWN);
				}
				break;
			case SDL_JOYBUTTONUP:  // Handle Joystick Button Presses
				if (windowEvent.jbutton.button == rotateButton_) 
				{
					//std::cout << "\n BUTTON " << (int) windowEvent.jbutton.button;
					InputDevice<PlayerEvent>::pushEvent(PLAYER_UN_ROTATE);
				}
				if (windowEvent.jbutton.button == downButton_) 
				{
					//std::cout << "\n BUTTON " << (int) windowEvent.jbutton.button;
					InputDevice<PlayerEvent>::pushEvent(PLAYER_UN_DOWN);
				}
				break;
			default:
				break;
			}
		}
    }

private:

	mw::JoystickPtr joystick_;
	int rotateButton_, downButton_;	
};

#endif // INPUTJOYSTICK_H