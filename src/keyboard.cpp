#include "keyboard.h"

#include <SDL.h>

Keyboard::Keyboard(std::string name,
	SDL_Keycode down,
	SDL_Keycode left,
	SDL_Keycode right,
	SDL_Keycode rotate,
	SDL_Keycode downGround) : Device(false),
	name_(name),
	down_(down),
	right_(right),
	left_(left),
	rotate_(rotate),
	downGround_(downGround)	{
}

Input Keyboard::currentInput() {
	return input_;
}

std::string Keyboard::getName() const {
	return name_;
}

void Keyboard::eventUpdate(const SDL_Event& windowEvent) {
	SDL_Keycode key = windowEvent.key.keysym.sym;

	switch (windowEvent.type) {
	case SDL_KEYDOWN:
		if (key == down_) {
			input_.down_ = true;
		} else if (key == left_) {
			input_.left_ = true;
		} else if (key == right_) {
			input_.right_ = true;
		} else if (key == rotate_) {
			input_.rotate_ = true;
		} else if (key == downGround_) {
			input_.downGround_ = true;
		}
		break;
	case SDL_KEYUP:
		if (key == down_) {
			input_.down_ = false;
		} else if (key == left_) {
			input_.left_ = false;
		} else if (key == right_) {
			input_.right_ = false;
		} else if (key == rotate_) {
			input_.rotate_ = false;
		} else if (key == downGround_) {
			input_.downGround_ = false;
		}
		break;
	default:
		break;
	}
}
