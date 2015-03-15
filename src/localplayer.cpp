#include "localplayer.h"
#include "actionhandler.h"

#include <string>
#include <functional>

LocalPlayer::LocalPlayer(int connectionId, int playerId, int width, int height,
	BlockType moving, BlockType next, const DevicePtr& device, PacketSender& sender) :
	Player(playerId, width, height, moving, next),
	sender_(sender),
	leftHandler_(0.09, false),
	rightHandler_(0.09, false),
	rotateHandler_(0.0, true),
	gravityMove_(1, false),				// Value doesn't matter! Changes every frame.
	downHandler_(0.04, false),
	device_(device),
	connectionId_(connectionId) {

	device_->update(getTetrisBoard());
	nbrOfUpdates_ = getTetrisBoard().getNbrOfUpdates();
	name_ = device_->getPlayerName();
	tetrisBoard_.addGameEventListener(std::bind(&LocalPlayer::boardListener,
		this, std::placeholders::_1, std::placeholders::_2));
}

void LocalPlayer::endGame() {
	if (!tetrisBoard_.isGameOver()) {
		update(Move::GAME_OVER);
	}
}

void LocalPlayer::update(Move move) {
	tetrisBoard_.update(move);
	if (sender_.isActive()) {
		net::Packet packet;
		packet << PacketType::PLAYER_MOVE;
		packet << connectionId_;
		packet << getId();
		packet << move;
		packet << tetrisBoard_.getNextBlockType();
		sender_.sendToAll(packet);
	}
}

void LocalPlayer::boardListener(GameEvent gameEvent, const TetrisBoard& board) {
	if (gameEvent == GameEvent::CURRENT_BLOCK_UPDATED) {
		// Generate a new block for a local player.
		tetrisBoard_.updateNextBlock(randomBlockType()); // The listener will be called again, but with GameEvent::NEXT_BLOCK_UPDATED.

		leftHandler_.reset();
		rightHandler_.reset();
		downHandler_.reset();
	}
	device_->update(board);
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

void LocalPlayer::restart(BlockType current, BlockType next) {
	tetrisBoard_.restart(current, next);
	levelUpCounter_ = 0;
	clearedRows_ = 0;
	level_ = 1;
	points_ = 0;
}

void LocalPlayer::resizeBoard(int width, int height) {
	tetrisBoard_.updateRestart(height, width, tetrisBoard_.getBlockType(), tetrisBoard_.getNextBlockType());
	levelUpCounter_ = 0;
	clearedRows_ = 0;
	level_ = 1;
	points_ = 0;
}

double LocalPlayer::calculateDownSpeed(int level) const {
	return 1 + level * 0.5;
}
