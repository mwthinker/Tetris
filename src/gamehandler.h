#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include "player.h"

#include <vector>
#include <memory>

class GameHandler {
public:
	virtual std::shared_ptr<TetrisBoard> createTetrisBoard(int width, int height, BlockType current, BlockType next) {
		return nullptr;
	}

	virtual void initGame(const std::vector<PlayerPtr>& players) = 0;

	virtual void eventHandler(const PlayerPtr& player, GameEvent gameEvent) = 0;

	virtual void countDown(int countDown) = 0;

protected:
	~GameHandler() {
	}
};

#endif // GAMEHANDLER_H
