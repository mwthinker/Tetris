#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"
#include "playerdata.h"

#include <mw/signal.h>

#include <string>
#include <memory>

class Player;
using PlayerPtr = std::shared_ptr<Player>;

class Player {
public:
	Player(int id, int width, int height, BlockType current, BlockType next);

	Player(int id, int width, int height, const PlayerData& playerData);
	
	virtual ~Player() = default;
	
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

	int getLastPositon() const {
		return lastPosition_;
	}

	void setLastPosition(int lastPosition) {
		lastPosition_ = lastPosition;
	}

	bool isGameOver() const {
		return tetrisBoard_.isGameOver();
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
	int lastPosition_;
};

#endif // PLAYER_H
