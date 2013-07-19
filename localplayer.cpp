#include "localplayer.h"
#include "actionhandler.h"

#include <string>
#include <memory>

LocalPlayer::LocalPlayer(int id, int width, int height, int maxLevel, const DevicePtr& device) : Player(id,width,height,maxLevel,false) {
	leftHandler_ = ActionHandler(0.10 , false);
	rightHandler_ = ActionHandler(0.10, false);
	rotateHandler_ = ActionHandler(0.0, true);

	// The time beetween each "gravity" move.
	double downTime = 1.0 / calculateDownSpeed(getLevel());
	gravityMove_ = ActionHandler(downTime, false);
	fastestDownTime_ = 0.040;
	downHandler_ = ActionHandler(1 / (1.0 / fastestDownTime_ - 1.0 / downTime), false);
	
	device_ = device;
	device_->update(getTetrisBoard());
}

void LocalPlayer::update(double deltaTime) {
    Input input = device_->currentInput();
	
	// The time beetween each "gravity" move.
	double downTime = 1.0 / calculateDownSpeed(getLevel());
	gravityMove_.setWaitingTime(downTime);
	downHandler_.setWaitingTime(1 / (1.0 / fastestDownTime_ - 1.0 / downTime));
	
	gravityMove_.update(deltaTime, true);
	if (gravityMove_.doAction()) {
		pushMove(Move::DOWN_GRAVITY);
	}

	leftHandler_.update(deltaTime, input.left_ && !input.right_);
	if (leftHandler_.doAction()) {
		pushMove(Move::LEFT);
	}

	rightHandler_.update(deltaTime, input.right_ && !input.left_);
	if (rightHandler_.doAction()) {
		pushMove(Move::RIGHT);
	}
	
	downHandler_.update(deltaTime, input.down_);
	if (downHandler_.doAction()) {
		pushMove(Move::DOWN);
	}

	rotateHandler_.update(deltaTime, input.rotate_);
	if (rotateHandler_.doAction()) {
		pushMove(Move::ROTATE_LEFT);
	}
}

void LocalPlayer::updateAi() {
	device_->update(getTetrisBoard());
}

double LocalPlayer::calculateDownSpeed(int level) const {
	return 1+level*0.5;
}

void LocalPlayer::polledGameEvent(GameEvent gameEvent) {
	switch (gameEvent) {
	case GameEvent::BLOCK_COLLISION:
		leftHandler_.reset();
		rightHandler_.reset();
		rotateHandler_.reset();
		downHandler_.reset();
		break;
	default:
		break;
	}
}
