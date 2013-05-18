#ifndef INPUTKEYBOARD_H
#define INPUTKEYBOARD_H

#include "device.h"

#include <SDL.h>

class InputKeyboard : public Device {
public:
	InputKeyboard(SDLKey down, SDLKey left, SDLKey right, SDLKey rotate) {		
		down_ = down;
		right_ = right;
		left_ = left;
		rotate_ = rotate;
	}

	Input currentInput() override {
		return input_;
	}

private:
	void eventUpdate(const SDL_Event& windowEvent) override {
		SDLKey key = windowEvent.key.keysym.sym;

        switch (windowEvent.type) {
        case SDL_KEYDOWN:
			if (key == down_) {
				input_.down = true;
			} else if (key == left_) {
				input_.left = true;
			} else if (key == right_) {
				input_.right = true;
			} else if (key == rotate_) {
				input_.rotate = true;
			}
            break;
        case SDL_KEYUP:
			if (key == down_) {
				input_.down = false;
			} else if (key == left_) {
				input_.left = false;
			} else if (key == right_) {
				input_.right = false;
			} else if (key == rotate_) {
				input_.rotate = false;
			}
            break;
        default:
            break;
        }
    }

	Input input_;

	SDLKey down_, right_, left_, rotate_;
};

#endif // INPUT_H
