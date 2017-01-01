#include "consolekeyboard.h"

ConsoleKeyboard::ConsoleKeyboard(std::string name, char down, char left, char right, char rotate, char downGround) : Device(false),
	name_(name),
	down_(down),
	right_(right),
	left_(left),
	rotate_(rotate),
	downGround_(downGround) {
}

void ConsoleKeyboard::eventUpdate(char key, double time) {
	input_.down_ = down_.isPressed(key, time);
	input_.right_ = left_.isPressed(key, time);
	input_.left_ = right_.isPressed(key, time);
	input_.rotate_ = rotate_.isPressed(key, time);
	input_.downGround_ = downGround_.isPressed(key, time);
}
