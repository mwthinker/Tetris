#include "player.h"
#include "gamegraphic.h"
#include "tetrisboard.h"

#include <string>
#include <queue>
#include <memory>

#include <random>

namespace {

	std::random_device rd;
	std::default_random_engine generator(rd());

	// Returns a random block type, which is not empty.
	BlockType generateBlockType() {
		static std::uniform_int_distribution<int> distribution(0, 6);
		static_assert((int) BlockType::EMPTY > 6 && (int) BlockType::WALL > 6, "BlockType::EMPTY should not be generated");
		return static_cast<BlockType>(distribution(generator));
	}

}

Player::Player(int id, int width, int height, bool remote, bool ai) : tetrisBoard_(height, width, BlockType::I, BlockType::I), ai_(ai), id_(id), remote_(remote) {
	if (!remote) {
		update(generateBlockType(), generateBlockType());
	}
	squaresPerLength_ = 0.8;
}

void Player::restart() {
	moves_ = std::queue<Move>();
	gameEvents_ = std::queue<GameEvent>();
	update(generateBlockType(), generateBlockType());
}

bool Player::updateBoard(Move& move, BlockType& next) {
	bool polled = pollMove(move);
	if (polled) {
		tetrisBoard_.update(move);
		GameEvent gameEvent;
		while (tetrisBoard_.pollGameEvent(gameEvent)) {
			gameEvents_.push(gameEvent);
			if (!remote_ && gameEvent == GameEvent::BLOCK_COLLISION) {
				// Generate a new block for a local player.
				tetrisBoard_.setNextBlockType(generateBlockType());
			}
		}
		next = tetrisBoard_.getNextBlockType();
		updateAi();
	}
	return polled;
}

void Player::update(Move move) {
	tetrisBoard_.update(move);
}

void Player::update(BlockType current, BlockType next) {
	tetrisBoard_.restart(current, next);
	level_ = 1;
}

void Player::update(Move move, BlockType next) {
	tetrisBoard_.setNextBlockType(next);
	tetrisBoard_.update(move);
}

void Player::update(const std::vector<BlockType>& blockTypes) {
	tetrisBoard_.addRows(blockTypes);
}

std::vector<BlockType> Player::generateRow() const {
	const unsigned int size = tetrisBoard_.getColumns();
	std::vector<bool> row(size);
	std::uniform_int_distribution<int> distribution(0, size - 1);

	for (unsigned int i = 0; i < size * squaresPerLength_; ++i) {
		int index = distribution(generator);
		unsigned int nbr = 0;
		while (nbr < size) {
			if (!row[(index + nbr) % size]) {
				row[(index + nbr) % size] = true;
				break;
			}
			++nbr;
		}
	}

	std::vector<BlockType> rows;
	for (unsigned int i = 0; i < size; ++i) {
		if (row[i]) {
			rows.push_back(generateBlockType());
		} else {
			rows.push_back(BlockType::EMPTY);
		}
	}
	return rows;
}

void Player::pushMove(Move move) {
	moves_.push(move);
}

bool Player::pollMove(Move& move) {
	if (moves_.empty()) {
		return false;
	}

	move = moves_.front();
	moves_.pop();
	return true;
}

int Player::getId() const {
	return id_;
}

void Player::setName(std::string name) {
	name_ = name;
}

std::string Player::getName() const {
	return name_;
}

const TetrisBoard& Player::getTetrisBoard() const {
	return tetrisBoard_;
}

void Player::setLevel(int level) {
	level_ = level;
}

int Player::getLevel() const {
	return level_;
}

BlockType Player::getCurrentBlock() const {
	return tetrisBoard_.getBlockType();
}

BlockType Player::getNextBlock() const {
	return tetrisBoard_.getNextBlockType();
}

bool Player::pollGameEvent(GameEvent& gameEvent) {
	if (gameEvents_.empty()) {
		return false;
	}

	gameEvent = gameEvents_.front();
	gameEvents_.pop();
	return true;
}
