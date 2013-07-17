#include "playerinfo.h"
#include "tetrisboard.h"
#include "graphicboard.h"

#include <string>
#include <memory>

PlayerInfo::PlayerInfo(int id, int width, int height, int maxLevel) : tetrisBoard_(height,width, BlockType::I, BlockType::I), graphicBoard_(tetrisBoard_), maxLevel_(maxLevel), id_(id) {
    reset();
}

PlayerInfo::~PlayerInfo() {
}

void PlayerInfo::setNbrOfClearedRows(int nbr) {
    nbrOfClearedRows_ = nbr;
    graphicBoard_.setNbrOfClearedRows(nbr);
}

int PlayerInfo::getNbrOfClearedRows() const {
    return nbrOfClearedRows_;
}

void PlayerInfo::setPoints(int points) {
    points_ = points;
    graphicBoard_.setPoints(points);
}

int PlayerInfo::getPoints() const {
    return points_;
}

void PlayerInfo::setLevel(int level) {
    level_ = level;
    graphicBoard_.setLevel(level_);
}

int PlayerInfo::getLevel() const  {
    return level_;
}

void PlayerInfo::setName(std::string name) {
    name_ = name;
    graphicBoard_.setName(name);
}

std::string PlayerInfo::getName() const {
    return name_;
}

void PlayerInfo::setGameOverMessage(std::string message) {
    graphicBoard_.setGameOverMessage(message);
}

void PlayerInfo::setCountDownMessage(std::string message) {
	graphicBoard_.setCountDownMessage(message);
}

void PlayerInfo::draw() {
    graphicBoard_.draw();
}

double PlayerInfo::getWidth() const {
    return graphicBoard_.getWidth();
}

double PlayerInfo::getHeight() const {
    return graphicBoard_.getHeight();
}

int PlayerInfo::getMaxLevel() const {
    return maxLevel_;
}

bool PlayerInfo::pollGameEvent(GameEvent& gameEvent) {
    return tetrisBoard_.pollGameEvent(gameEvent);
}

int PlayerInfo::getId() const {
    return id_;
}

const TetrisBoard& PlayerInfo::getTetrisBoard() const {
    return tetrisBoard_;
}

void PlayerInfo::setLevelUpCounter(int value) {
    levelUpCounter_ = value;
}

int PlayerInfo::getLevelUpCounter() const {
    return levelUpCounter_;
}

void PlayerInfo::triggerGameOverEvent() {
    tetrisBoard_.triggerGameOverEvent();
}

BlockType PlayerInfo::getCurrentBlock() const {
    return tetrisBoard_.currentBlock().blockType();
}

BlockType PlayerInfo::getNextBlock() const {
    return tetrisBoard_.nextBlock().blockType();
}

void PlayerInfo::reset() {
    level_ = 1;
    points_ = 0;
    nbrOfClearedRows_ = 0;
    levelUpCounter_ = 0;
    graphicBoard_.setPoints(points_);
    graphicBoard_.setLevel(level_);
    graphicBoard_.setNbrOfClearedRows(nbrOfClearedRows_);
    graphicBoard_.setGameOverMessage("Game over!");
}
