#ifndef TETRISGAMEEVENT_H
#define TETRISGAMEEVENT_H

#include <vector>

#include "player.h"

class Connection;

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

class CountDown : public TetrisGameEvent {
public:
	CountDown(int timeLeft) : timeLeft_(timeLeft) {
	}

	int timeLeft_;
};

class GamePause : public TetrisGameEvent {
public:
	GamePause(bool pause) : pause_(pause) {
	}

	bool pause_;
};

class GameOver : public TetrisGameEvent {
public:
    GameOver(unsigned int position, const std::shared_ptr<Player>& player) : position_(position), player_(player) {
    }

    unsigned int position_;
	std::shared_ptr<Player> player_;
};

class InitGame : public TetrisGameEvent {
public:
	InitGame(const std::vector<std::shared_ptr<Player>>& players, const std::vector<std::shared_ptr<Connection>>& remoteConnections)
		: players_(players), remoteConnections_(remoteConnections) {
	}

	std::vector<std::shared_ptr<Player>> players_;
	std::vector<std::shared_ptr<Connection>> remoteConnections_;
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
