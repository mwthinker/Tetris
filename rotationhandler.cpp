#include "rotationhandler.h"

// member functions declared public ----------------

RotationHandler::RotationHandler() {
	action_ = false;
	lastAction_ = false;
}

void RotationHandler::update(double timeStep, bool action) {
	if (action && lastAction_!=action) {
		action_ = true;
	}
	lastAction_ = action;
}

bool RotationHandler::doAction() {
	if (action_ ) {
		action_ = false;
		return true;
	}
	return false;
}
