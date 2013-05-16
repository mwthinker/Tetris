#include "localplayer.h"
#include "actionhandler.h"
#include "movehandler.h"
#include "rotationhandler.h"

#include <string>
#include <memory>

LocalPlayer::LocalPlayer(int id, const DevicePtr& device) : Player(id) {
	int nbrOfRows = 20;
	leftHandler_ = new MoveHandler(0.09); // 0.12
	rightHandler_ = new MoveHandler(0.09); // 0.12
	downHandler_ = new MoveHandler(nbrOfRows/20.0 * 0.05);
	rotateHandler_ = new RotationHandler();

	time_ = 0.0;
	lastDownTime_ = 0.0;
	device_ = device;
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
		pushMove(TetrisBoard::DOWN_GRAVITY);
	}

	leftHandler_->update(deltaTime,input.left && !input.right);
	if (leftHandler_->doAction()) {
		pushMove(TetrisBoard::LEFT);
	}

	rightHandler_->update(deltaTime,input.right && !input.left);
	if (rightHandler_->doAction()) {
		pushMove(TetrisBoard::RIGHT);
	}

	downHandler_->update(deltaTime,input.down);
	if (downHandler_->doAction()) {
		pushMove(TetrisBoard::DOWN);
	}

	rotateHandler_->update(deltaTime,input.rotate);
	if (rotateHandler_->doAction()) {
		pushMove(TetrisBoard::ROTATE_LEFT);
	}
}

double LocalPlayer::calculateDownSpeed(int level) const {
	return 1+level*0.5;
}

