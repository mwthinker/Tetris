#include "playerinfo.h"
#include "tetrisboard.h"

PlayerInfo::PlayerInfo(int width, int height) : tetrisBoard_(height,width, BlockType::I, BlockType::I) {
}

PlayerInfo::~PlayerInfo() {
}

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
    return tetrisBoard_.currentBlock().blockType();
}

BlockType PlayerInfo::getNextBlock() const {
    return tetrisBoard_.nextBlock().blockType();
}
