#include "player.h"
#include "graphicboard.h"
#include "tetrisboard.h"

#include <string>
#include <queue>
#include <memory>

#include <random>

namespace {

	std::random_device rd;
	std::default_random_engine generator(rd());

	BlockType generateBlockType() {
		std::uniform_int_distribution<int> distribution(0,6);
		int nbr = distribution(generator);
		BlockType blockType = BlockType::EMPTY;
		switch (nbr) {
		case 0:
			blockType = BlockType::I;
			break;
		case 1:
			blockType = BlockType::J;
			break;
		case 2:
			blockType = BlockType::L;
			break;
		case 3:
			blockType = BlockType::O;
			break;
		case 4:
			blockType = BlockType::S;
			break;
		case 5:
			blockType = BlockType::T;
			break;
		case 6:
			blockType = BlockType::Z;
			break;
		}

		return blockType;
	}

}

Player::Player(int id, int width, int height, bool remote, bool ai) : PlayerInfo(width,height), ai_(ai), id_(id) {
	if (!remote) {
		update(generateBlockType(), generateBlockType());
	}
	squaresPerLength_ = 0.8;
}

void Player::restart() {
    moves_ = std::queue<Move>();
	update(generateBlockType(),generateBlockType());
}

bool Player::updateBoard(Move& move, BlockType& next) {
    Move polledMove;
    bool polled = pollMove(polledMove);
    if (polled) {
		BlockType blockType = generateBlockType();
		next = blockType;
		tetrisBoard_.add(blockType);
        move = polledMove;
        update(move);
		updateAi();
    }
    return polled;
}

void Player::update(Move move) {
    tetrisBoard_.update(move);
}

void Player::update(BlockType current, BlockType next) {
	tetrisBoard_ .restart(current, next);
}

void Player::update(Move move, BlockType next) {
    tetrisBoard_.add(next);
    tetrisBoard_.update(move);
}

void Player::update(const std::vector<BlockType>& blockTypes) {
    tetrisBoard_.addRows(blockTypes);
}

std::vector<BlockType> Player::generateRow() const {
	// Creates a row represented by a vector filled with true or false.
	// True means it is filled with a square. False means empty.
	// The percentage of true per row is squaresPerLength_.
	const unsigned int size = tetrisBoard_.getNbrOfColumns();
	std::vector<bool> row(size);
	std::uniform_int_distribution<int> distribution(0,size-1);

	for (unsigned int i = 0; i <  size * squaresPerLength_; ++i) {
		int index = distribution(generator);
		unsigned int nbr = 0;
		while (nbr < size) {
			if (!row[ (index+nbr) % size]) {
				row[ (index+nbr) % size] = true;
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
