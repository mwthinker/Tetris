#include "player.h"

Player::Player(int id, int width, int height, BlockType current, BlockType next) :
	tetrisBoard_(width, height, current, next),
	id_(id), level_(1), points_(0), lastPosition_(0) {

}

Player::Player(int id, int width, int height, const PlayerData& playerData) :
	tetrisBoard_(playerData.board_, width, height, playerData.current_,
		playerData.next_, playerData.clearedRows_), id_(id),
	level_(playerData.level_), points_(playerData.points_),
	lastPosition_(playerData.lastPosition_) {
}

mw::signals::Connection Player::addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback) {
	return tetrisBoard_.addGameEventListener(callback);
}
