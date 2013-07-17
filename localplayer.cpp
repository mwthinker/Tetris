#include "localplayer.h"
#include "actionhandler.h"
#include "movehandler.h"
#include "rotationhandler.h"

#include <string>
#include <memory>

LocalPlayer::LocalPlayer(int id, int width, int height, int maxLevel, const DevicePtr& device) : Player(id,width,height,maxLevel,false) {
	int nbrOfRows = 20;
	leftHandler_ = new MoveHandler(0.09); // 0.12
	rightHandler_ = new MoveHandler(0.09); // 0.12
	downHandler_ = new MoveHandler(nbrOfRows/20.0 * 0.055);
	rotateHandler_ = new RotationHandler();

	time_ = 0.0;
	lastDownTime_ = 0.0;
	device_ = device;
	device_->update(getTetrisBoard());
}

LocalPlayer::~LocalPlayer() {
	delete leftHandler_;
	delete rightHandler_;
	delete downHandler_;
	delete rotateHandler_;
}

void LocalPlayer::update(double deltaTime) {
    Input input = device_->currentInput();

	time_ += deltaTime;
	double downTime = 1.0 / calculateDownSpeed(getLevel()); // The time beetween each "gravity" move.

	if (lastDownTime_ + downTime < time_) {
		lastDownTime_ = time_;
		pushMove(TetrisBoard::Move::DOWN_GRAVITY);
	}

	leftHandler_->update(deltaTime, input.left_ && !input.right_);
	if (leftHandler_->doAction()) {
		pushMove(TetrisBoard::Move::LEFT);
	}

	rightHandler_->update(deltaTime, input.right_ && !input.left_);
	if (rightHandler_->doAction()) {
		pushMove(TetrisBoard::Move::RIGHT);
	}

    // This calulates a new deltaTime so that the downHandler and gravity together has the max speed of the downHandler.
	double newDeltaTime = deltaTime / (1.0 - downHandler_->getWaitingTime() / downTime);
	downHandler_->update(newDeltaTime,input.down_);
	if (downHandler_->doAction()) {
		pushMove(TetrisBoard::Move::DOWN);
	}

	rotateHandler_->update(deltaTime, input.rotate_);
	if (rotateHandler_->doAction()) {
		pushMove(TetrisBoard::Move::ROTATE_LEFT);
	}
}

void LocalPlayer::updateAi() {
	device_->update(getTetrisBoard());
}

double LocalPlayer::calculateDownSpeed(int level) const {
	return 1+level*0.5;
}
