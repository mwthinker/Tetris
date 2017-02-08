#include "player.h"

Player::Player(int id, int width, int height, BlockType current, BlockType next) :
	tetrisBoard_(height, width, current, next),
	id_(id), level_(1), points_(0) {

}

Player::Player(int id, int width, int height, int points, int level, Block current, BlockType next, const std::vector<BlockType>& board) : 
	tetrisBoard_(board, height, width, current, next), id_(id), level_(level), points_(points) {

}

mw::signals::Connection Player::addGameEventListener(
	const std::function<void(GameEvent, const TetrisBoard&)>& callback) {

	return tetrisBoard_.addGameEventListener(callback);
}
