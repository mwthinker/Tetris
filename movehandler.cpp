#include "movehandler.h"

// member functions declared public ----------------

MoveHandler::MoveHandler(double waitingTime) {
	lastTime_ = 0;
    waitingTime_ = waitingTime;
    time_ = 0;
	action_ = false;
}

void MoveHandler::update(double timeStep, bool action) {
    time_ += timeStep;
    if (action) {
		if (time_ - lastTime_ > waitingTime_) {
			lastTime_ = time_;
			action_ = true;
		}
	}
}

bool MoveHandler::doAction() {
    if (action_ ) {
		action_ = false;
		return true;
	}
	return false;
}

double MoveHandler::getWaitingTime() const {
    return waitingTime_;
}
