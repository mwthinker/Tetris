#ifndef INPUTKEYBOARD_H
#define INPUTKEYBOARD_H

#include "inputdevice.h"
#include "human.h"

class InputKeyboard : public InputDevice<PlayerEvent> {
public:
	InputKeyboard(SDLKey down, SDLKey left, SDLKey right, SDLKey rotate) {		
		down_ = down;
		right_ = right;
		left_ = left;
		rotate_ = rotate;		
	}

    void eventUpdate(const SDL_Event& windowEvent) {
		SDLKey key = windowEvent.key.keysym.sym;
		
        switch (windowEvent.type) {
        case SDL_KEYDOWN:			
			if (key == down_) {
				pushEvent(PLAYER_DOWN);
			} else if (key == left_) {
				pushEvent(PLAYER_LEFT);
			} else if (key == right_) {
				pushEvent(PLAYER_RIGHT);
			} else if (key == rotate_) {
				pushEvent(PLAYER_ROTATE);
			}
            break;
        case SDL_KEYUP:            
			if (key == down_) {
				pushEvent(PLAYER_UN_DOWN);
			} else if (key == left_) {
				pushEvent(PLAYER_UN_LEFT);
			} else if (key == right_) {
				pushEvent(PLAYER_UN_RIGHT);
			} else if (key == rotate_) {
				pushEvent(PLAYER_UN_ROTATE);
			}
            break;
        default:
            break;
        }
    }

private:
	SDLKey down_, right_, left_, rotate_;
};

#endif // INPUTKEYBOARD_H