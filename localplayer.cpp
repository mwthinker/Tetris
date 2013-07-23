#include "localplayer.h"
#include "actionhandler.h"

#include <string>
#include <memory>

LocalPlayer::LocalPlayer(int id, int width, int height, const DevicePtr& device) : Player(id, width,height ,false) {
	leftHandler_ = ActionHandler(0.09, false);
	rightHandler_ = ActionHandler(0.09, false);
	rotateHandler_ = ActionHandler(0.0, true);

	// The time beetween each "gravity" move.
	double downTime = 1.0 / calculateDownSpeed(getLevel());
	gravityMove_ = ActionHandler(downTime, false);
	downHandler_ = ActionHandler(0.04, false);

	device_ = device;
	device_->update(getTetrisBoard());
}

void LocalPlayer::update(double deltaTime) {
    Input input = device_->currentInput();
	
	// The time beetween each "gravity" move.
	double downTime = 1.0 / calculateDownSpeed(getLevel());
	gravityMove_.setWaitingTime(downTime);
	
	gravityMove_.update(deltaTime, true);
	if (gravityMove_.doAction()) {
		downHandler_.reset();
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
