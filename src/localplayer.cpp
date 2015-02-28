#include "localplayer.h"
#include "actionhandler.h"

#include <string>
#include <functional>

LocalPlayer::LocalPlayer(int id, int width, int height, const DevicePtr& device) :
	Player(id, width, height),
	leftHandler_(ActionHandler(0.09, false)),
	rightHandler_(ActionHandler(0.09, false)),
	rotateHandler_(ActionHandler(0.0, true)),
	
	gravityMove_(ActionHandler(1, false)), // Value doesn't matter! Changes every frame.
	downHandler_(ActionHandler(0.04, false)),
	device_(device) {
	
	device_->update(getTetrisBoard());
	nbrOfUpdates_ = getTetrisBoard().getNbrOfUpdates();
	name_ = device_->getPlayerName();
	tetrisBoard_.addGameEventListener(std::bind(&LocalPlayer::boardListener, this, std::placeholders::_1, std::placeholders::_2));
}

void LocalPlayer::update(Move move) {
	tetrisBoard_.update(move);
	packet_ << move;
	packet_ << tetrisBoard_.getNextBlockType();
}

void LocalPlayer::boardListener(GameEvent gameEvent, const TetrisBoard& board) {
	if (gameEvent == GameEvent::CURRENT_BLOCK_UPDATED) {		
		// Generate a new block for a local player.
		BlockType type = randomBlockType();
		tetrisBoard_.updateNextBlock(type); // The listener will be called again, but with GameEvent::NEXT_BLOCK_UPDATED.
		
		leftHandler_.reset();
		rightHandler_.reset();
		downHandler_.reset();
		device_->update(board);
	}
}

void LocalPlayer::update(double deltaTime) {
	Input input = device_->currentInput();

	// The time beetween each "gravity" move.
	double downTime = 1.0 / calculateDownSpeed(getLevel());
	gravityMove_.setWaitingTime(downTime);

	gravityMove_.update(deltaTime, true);
	if (gravityMove_.doAction()) {
		update(Move::DOWN_GRAVITY);
	}

	leftHandler_.update(deltaTime, input.left_ && !input.right_);
	if (leftHandler_.doAction()) {
		update(Move::LEFT);
	}

	rightHandler_.update(deltaTime, input.right_ && !input.left_);
	if (rightHandler_.doAction()) {
		update(Move::RIGHT);
	}

	downHandler_.update(deltaTime, input.down_);
	if (downHandler_.doAction()) {
		update(Move::DOWN);
	}

	rotateHandler_.update(deltaTime, input.rotate_);
	if (rotateHandler_.doAction()) {
		update(Move::ROTATE_LEFT);
	}
}

bool LocalPlayer::pollPacket(net::Packet& packet) {
	if (packet_.getSize() > 1) {
		packet << packet_;
		packet_ = net::Packet();
		return true;
	}
	return false;
}

double LocalPlayer::calculateDownSpeed(int level) const {
	return 1 + level * 0.5;
}
