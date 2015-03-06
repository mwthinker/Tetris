#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"

#include <mw/signal.h>

#include <string>
#include <memory>

class Player {
public:
	Player(int id, int width, int height, BlockType moving, BlockType next);
	
	inline virtual ~Player() {
	}
	
	// Update the player.
	virtual void update(double deltaTime) = 0;
	
	inline int getId() const {
		return id_;
	}
	
	std::string getName() const {
		return name_;
	}
	
	inline int getLevel() const {
		return level_;
	}

	inline int getPoints() const {
		return points_;
	}
	
	const TetrisBoard& getTetrisBoard() const {
		return tetrisBoard_;
	}

	mw::signals::Connection addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback);

protected:
	TetrisBoard tetrisBoard_;
	std::string name_;
	int id_;
	int level_;
	int points_, nbrClearedRows_, levelUpCounter_;
};

#endif // PLAYER_H
