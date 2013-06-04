#include "player.h"
#include "graphicboard.h"
#include "tetrisboard.h"

#include <string>
#include <queue>
#include <memory>

Player::Player(int id, int width, int height, int maxLevel, bool remote) : PlayerInfo(id,width,height,maxLevel) {
    tetrisBoard_.setDecideRandomBlockType(!remote);
}

Player::~Player() {
}

void Player::restart() {
    moves_ = std::queue<TetrisBoard::Move>();
    bool random = tetrisBoard_.isDecideRandomBlockType();
    tetrisBoard_ = TetrisBoard(tetrisBoard_.getNbrOfRows(),tetrisBoard_.getNbrOfColumns());
    tetrisBoard_.setDecideRandomBlockType(random);
    reset();
}

bool Player::updateBoard(TetrisBoard::Move& move, BlockType& next) {
    TetrisBoard::Move polledMove;
    bool polled = pollMove(polledMove);
    if (polled) {
        next = tetrisBoard_.nextBlock().blockType();
        move = polledMove;
        update(move);
    }
    return polled;
}

void Player::update(TetrisBoard::Move move) {
    tetrisBoard_.update(move);
}

void Player::update(BlockType current, BlockType next) {
    tetrisBoard_.setNonRandomCurrentBlockType(current);
    tetrisBoard_.setNonRandomNextBlockType(next);
}

void Player::update(TetrisBoard::Move move, BlockType next) {
    tetrisBoard_.setNonRandomNextBlockType(next);
    tetrisBoard_.update(move);
}

void Player::update(const std::vector<BlockType>& blockTypes) {
    tetrisBoard_.addRows(blockTypes);
}

void Player::pushMove(TetrisBoard::Move move) {
    moves_.push(move);
}

bool Player::pollMove(TetrisBoard::Move& move) {
    if (moves_.empty()) {
        return false;
    }

    move = moves_.front();
    moves_.pop();
    return true;
}
