#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include "player.h"

#include <memory>

class GameHandler {
public:
	virtual void initGame(std::vector<std::shared_ptr<Player>>& players) = 0;

	virtual void eventHandler(const std::shared_ptr<Player>& player, GameEvent gameEvent) = 0;

	virtual void countDown(int countDown) = 0;

protected:
	~GameHandler() {
	}
};

#endif // GAMEHANDLER_H
