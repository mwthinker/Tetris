#include "player.h"

Player::Player(int id, int width, int height, BlockType current, BlockType next) :
	tetrisBoard_(height, width, current, next),
	id_(id), level_(1), points_(0), clearedRows_(0) {

}

mw::signals::Connection Player::addGameEventListener(
	const std::function<void(GameEvent, const TetrisBoard&)>& callback) {

	return tetrisBoard_.addGameEventListener(callback);
}
