#include "playerinfo.h"
#include "tetrisboard.h"

bool PlayerInfo::pollGameEvent(GameEvent& gameEvent) {
	return tetrisBoard_.pollGameEvent(gameEvent);
}

const TetrisBoard& PlayerInfo::getTetrisBoard() const {
	return tetrisBoard_;
}

void PlayerInfo::triggerGameOverEvent() {
	tetrisBoard_.triggerGameOverEvent();
}

BlockType PlayerInfo::getCurrentBlock() const {
	return tetrisBoard_.getBlockType();
}

BlockType PlayerInfo::getNextBlock() const {
	return tetrisBoard_.getBlockType();
}

void PlayerInfo::setLevel(int level) {
	level_ = level;
}

int PlayerInfo::getLevel() const {
	return level_;
}

void PlayerInfo::setPoints(int points) {
	points_ = points;
}

int PlayerInfo::getPoints() const {
	return points_;
}

void PlayerInfo::addPoints(int points) {
	points_ += points;
}

void PlayerInfo::setNbrClearedRows(int nbrClearedRows) {
	nbrClearedRows_ = nbrClearedRows;
}

int PlayerInfo::getNbrClearedRows() const {
	return nbrClearedRows_;
}

void PlayerInfo::addNbrClearedRows(int nbrClearedRows) {
	nbrClearedRows_ += nbrClearedRows;
}

void PlayerInfo::setLevelUpCounter(int levelUpCounter) {
	levelUpCounter_ = levelUpCounter;
}

int PlayerInfo::getLevelUpCounter() const {
	return levelUpCounter_;
}

void PlayerInfo::addLevelUpCounter(int levelUpCounter) {
	levelUpCounter_ += levelUpCounter;
}

void PlayerInfo::setName(std::string name) {
	name_ = name;
}

std::string PlayerInfo::getName() const {
	return name_;
}

PlayerInfo::PlayerInfo(int width, int height) : tetrisBoard_(height, width, BlockType::I, BlockType::I), 
	level_(1), points_(0), nbrClearedRows_(0), levelUpCounter_(0) {
}
