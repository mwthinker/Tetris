#include "actionhandler.h"

ActionHandler::ActionHandler() {
}

ActionHandler::ActionHandler(double waitingTime, bool rebound) {
	lastTime_ = 0;
    waitingTime_ = waitingTime;
    time_ = 0;
	action_ = false;
	rebound_ = rebound;
	lastAction_ = false;
}

void ActionHandler::update(double timeStep, bool action) {
    time_ += timeStep;
    if (action) {
		if ( time_ - lastTime_ >= waitingTime_ && ( !rebound_ || (rebound_ && !lastAction_) ) ) {
			lastTime_ = time_;
			action_ = true;
		}
	}
	lastAction_ = action;
}

bool ActionHandler::doAction() {
    if (action_ ) {
		action_ = false;
		return true;
	}
	return false;
}

double ActionHandler::getWaitingTime() const {
    return waitingTime_;
}

void ActionHandler::setWaitingTime(double waitingTime) {
	waitingTime_ = waitingTime;
}

void ActionHandler::reset() {
	lastTime_ = time_;
}
