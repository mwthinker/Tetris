#include "consolekeyboard.h"

ConsoleKeyboard::ConsoleKeyboard(std::string name, console::Key down, console::Key left, console::Key right, console::Key rotate, console::Key downGround) : Device(false),
	name_(name),
	down_(down),
	right_(right),
	left_(left),
	rotate_(rotate),
	downGround_(downGround) {
}

void ConsoleKeyboard::eventUpdate(const console::ConsoleEvent& consoleEvent) {
	switch (consoleEvent.type) {
		case console::ConsoleEventType::KEYDOWN:
			if (down_ == consoleEvent.keyEvent.key) {
				input_.down_ = true;
			} else if (right_ == consoleEvent.keyEvent.key) {
				input_.right_ = true;
			} else if (left_ == consoleEvent.keyEvent.key) {
				input_.left_ = true;
			} else if (rotate_ == consoleEvent.keyEvent.key) {
				input_.rotate_ = true;
			} else if (downGround_ == consoleEvent.keyEvent.key) {
				input_.downGround_ = true;
			}
			break;
		case console::ConsoleEventType::KEYUP:
			if (down_ == consoleEvent.keyEvent.key) {
				input_.down_ = false;
			} else if (right_ == consoleEvent.keyEvent.key) {
				input_.right_ = false;
			} else if (left_ == consoleEvent.keyEvent.key) {
				input_.left_ = false;
			} else if (rotate_ == consoleEvent.keyEvent.key) {
				input_.rotate_ = false;
			} else if (downGround_ == consoleEvent.keyEvent.key) {
				input_.downGround_ = true;
			}
			break;
	}
}
