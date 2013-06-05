#include "keyboard.h"

#include <SDL.h>

Keyboard::Keyboard(std::string name, SDLKey down, SDLKey left, SDLKey right, SDLKey rotate) {		
	name_ = name;
	down_ = down;
	right_ = right;
	left_ = left;
	rotate_ = rotate;
}

Input Keyboard::currentInput() {
	return input_;
}

std::string Keyboard::getName() const {
	return name_;
}

void Keyboard::eventUpdate(const SDL_Event& windowEvent) {
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
