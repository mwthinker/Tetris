#include "human.h"

#include "player.h"
#include "actionhandler.h"
#include "movehandler.h"
#include "rotationhandler.h"

#include <string>
#include <memory>

// public

Human::Human() {
	int nbrOfRows = 20;
	leftHandler_ = new MoveHandler(0.09); // 0.12
	rightHandler_ = new MoveHandler(0.09); // 0.12
	downHandler_ = new MoveHandler(nbrOfRows/20.0 * 0.05);
	rotateHandler_ = new RotationHandler();

	time_ = 0.0;
	lastDownTime_ = 0.0;

	input_.down = false;
	input_.rotate = false;
	input_.left = false;
	input_.right = false;
}

Human::~Human() {
	delete leftHandler_;
	delete rightHandler_;
	delete downHandler_;
	delete rotateHandler_;
}

void Human::updatePlayerEvent(PlayerEvent playerEvent) {
	switch (playerEvent) {
	case PLAYER_ROTATE:
		input_.rotate = true;
		break;
	case PLAYER_DOWN:
		input_.down = true;
		break;
	case PLAYER_LEFT:
		input_.left = true;
		break;
	case PLAYER_RIGHT:
		input_.right = true;
		break;
	case PLAYER_UN_ROTATE:
		input_.rotate = false;
		break;
	case PLAYER_UN_DOWN:
		input_.down = false;
		break;
	case PLAYER_UN_LEFT:
		input_.left = false;
		break;
	case PLAYER_UN_RIGHT:
		input_.right = false;
		break;
	}
}    

void Human::update(double deltaTime, int level) {
	time_ += deltaTime;
	double downTime = 1.0 / calculateDownSpeed(level); // The time beetween each "gravity" move.

	if (lastDownTime_ + downTime < time_) {
		lastDownTime_ = time_;
		pushMove(TetrisBoard::DOWN_GRAVITY);
	}

	leftHandler_->update(deltaTime,input_.left && !input_.right);
	if (leftHandler_->doAction()) {
		pushMove(TetrisBoard::LEFT);
	}

	rightHandler_->update(deltaTime,input_.right && !input_.left);
	if (rightHandler_->doAction()) {
		pushMove(TetrisBoard::RIGHT);
	}

	downHandler_->update(deltaTime,input_.down);
	if (downHandler_->doAction()) {
		pushMove(TetrisBoard::DOWN);
	}

	rotateHandler_->update(deltaTime,input_.rotate);
	if (rotateHandler_->doAction()) {
		pushMove(TetrisBoard::ROTATE_LEFT);
	}
}

bool Human::pollMove(TetrisBoard::Move& move) {
	if (moves_.empty()) {
		return false;
	}

	move = moves_.front();
	moves_.pop();
	return true;
}
// private
double Human::calculateDownSpeed(int level) const {
	return 1+level*0.5;
}

void Human::pushMove(TetrisBoard::Move move) {
	moves_.push(move);
}