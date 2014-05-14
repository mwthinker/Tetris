#ifndef GAMEHANDLER_H
#define GAMEHANDLER_H

#include "player.h"

class GameHandler {
public:
	virtual void initGame(const std::vector<PlayerPtr>& players) = 0;

	virtual void eventHandler(const PlayerPtr& player, GameEvent gameEvent) = 0;

	virtual void countDown(int countDown) = 0;

protected:
	~GameHandler() {
	}
};

#endif // GAMEHANDLER_H
