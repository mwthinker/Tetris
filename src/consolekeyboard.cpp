#include "consolekeyboard.h"

ConsoleKeyboard::ConsoleKeyboard(std::string name, char down, char left, char right, char rotate, char downGround) : Device(false),
	name_(name),
	down_(down),
	right_(right),
	left_(left),
	rotate_(rotate),
	downGround_(downGround) {
}

void ConsoleKeyboard::eventUpdate(char key) {
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
}
