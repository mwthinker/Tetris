#ifndef TETRISGAMEEVENT_H
#define TETRISGAMEEVENT_H

#include <vector>

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

#endif // TETRISGAMEEVENT_H
