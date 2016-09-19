#ifndef TETRISGAMEEVENT_H
#define TETRISGAMEEVENT_H

#include <vector>

#include "player.h"

class TetrisGameEvent {
public:
	virtual ~TetrisGameEvent() {
	}
};

class NewConnection : public TetrisGameEvent {
public:
};

class GameStart : public TetrisGameEvent {
public:
	enum Status {
		LOCAL,
		SERVER,
		CLIENT
	};

	GameStart(Status status) : status_(status) {
	}	

	Status status_;
};

class GamePause : public TetrisGameEvent {
public:
	GamePause(bool pause) : pause_(pause) {
	}

	bool pause_;
};

class GameOver : public TetrisGameEvent {
public:
    GameOver(int points) : points_(points) {
    }

    int points_;
};

class InitGame : public TetrisGameEvent {
public:
	InitGame(const std::vector<std::shared_ptr<Player>>& players) : players_(players) {
	}

	std::vector<std::shared_ptr<Player>> players_;
};

class LevelChange : public TetrisGameEvent {
public:
	LevelChange(const std::shared_ptr<Player>& player, int newLevel, int oldLevel) : player_(player), newLevel_(newLevel), oldLevel_(oldLevel) {
	}

	int newLevel_;
	int oldLevel_;
	std::shared_ptr<Player> player_;
};

class PointsChange : public TetrisGameEvent {
public:
	PointsChange(const std::shared_ptr<Player>& player, int newPoints, int oldPoints) : player_(player), newPoints_(newPoints), oldPoints_(oldPoints) {
	}

	int newPoints_;
	int oldPoints_;
	std::shared_ptr<Player> player_;
};

#endif // TETRISGAMEEVENT_H
