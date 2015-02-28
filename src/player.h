#ifndef PLAYER_H
#define PLAYER_H

#include "tetrisboard.h"

#include <mw/signal.h>

#include <string>
#include <memory>

class Player {
public:
	Player(int id, int width, int height);
	
	virtual ~Player() {
	}
	
	// Update the player.
	virtual void update(double deltaTime) = 0;

	// Return the player's id.
	int getId() const;
	
	std::string getName() const;
	
	int getLevel() const;

	int getPoints() const;

	// Get the tetrisboard.
	const TetrisBoard& getTetrisBoard() const;

	mw::signals::Connection addGameEventListener(const std::function<void(GameEvent, const TetrisBoard&)>& callback);

protected:
	TetrisBoard tetrisBoard_;
	int id_;
	std::string name_;
	int level_;
	int points_, nbrClearedRows_, levelUpCounter_;
};

#endif // PLAYER_H
