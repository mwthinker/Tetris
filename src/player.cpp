#include "player.h"

Player::Player(int id, int width, int height) : 
	tetrisBoard_(height, width, BlockType::I, BlockType::I), 
	id_(id), level_(1), points_(0), nbrClearedRows_(0), levelUpCounter_(0) {

}

int Player::getId() const {
	return id_;
}

std::string Player::getName() const {
	return name_;
}

const TetrisBoard& Player::getTetrisBoard() const {
	return tetrisBoard_;
}

mw::signals::Connection Player::addGameEventListener(
	const std::function<void(GameEvent, const TetrisBoard&)>& callback) {

	return tetrisBoard_.addGameEventListener(callback);
}

int Player::getLevel() const {
	return level_;
}
