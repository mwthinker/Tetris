#include "consolekeyboard.h"

#include <iostream>

ConsoleKeyboard::ConsoleKeyboard(std::string name, 
	console::Key down, console::Key left, console::Key right, 
	console::Key rotate, console::Key downGround) :
	name_(name),
	down_(down),
	right_(right),
	left_(left),
	rotate_(rotate),
	downGround_(downGround) {
}

void ConsoleKeyboard::eventUpdate(const console::ConsoleEvent& consoleEvent) {
	console::Key key = consoleEvent.keyEvent.key;
	switch (consoleEvent.type) {
		case console::ConsoleEventType::KEYDOWN:
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
		case console::ConsoleEventType::KEYUP:
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
