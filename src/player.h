#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"

#include <mw/signal.h>

#include <string>
#include <memory>

class Player;
using PlayerPtr = std::shared_ptr<Player>;

class Player {
public:
	Player(int id, int width, int height, BlockType current, BlockType next);

	Player(int id, int width, int height, int points, int level, Block current, BlockType next, const std::vector<BlockType>& board);
	
	virtual ~Player() {
	}
	
	// Update the player.
	virtual void update(double deltaTime) = 0;
	
	int getId() const {
		return id_;
	}
	
	std::string getName() const {
		return name_;
	}
	
	int getLevel() const {
		return level_;
	}

	int getPoints() const {
		return points_;
	}

	int getClearedRows() const {
		return tetrisBoard_.getRemovedRows();
	}
	
	const TetrisBoard& getTetrisBoard() const {
		return tetrisBoard_;
	}

	float getGravityDownSpeed() const {
		return 1 + level_ * 0.5f;
	}

	float getWaitingTime() const {
		return 0.5f;
	}

	virtual bool isAi() const = 0;

	mw::signals::Connection addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback);

protected:
	TetrisBoard tetrisBoard_;
	std::string name_;
	int id_;
	int level_;
	int points_;
};

#endif // PLAYER_H
