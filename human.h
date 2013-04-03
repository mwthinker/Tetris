#ifndef HUMAN_H
#define HUMAN_H

#include "player.h"
#include "actionhandler.h"

#include <memory>

enum PlayerEvent {PLAYER_ROTATE,PLAYER_DOWN,PLAYER_LEFT,PLAYER_RIGHT,
                  PLAYER_UN_ROTATE,PLAYER_UN_DOWN,PLAYER_UN_LEFT,PLAYER_UN_RIGHT};

struct Input {
	bool rotate;
	bool down;
	bool left;
	bool right;
};

class Human {
public:
    Human();
    ~Human();

    void updatePlayerEvent(PlayerEvent playerEvent);
    void update(double deltaTime, int level);
	bool pollMove(TetrisBoard::Move& move);
private:
	double calculateDownSpeed(int level) const;
	void pushMove(TetrisBoard::Move move);

    Input input_;

    double time_;						// Gametime
	double lastDownTime_;				// The gametime for the last "gravity" move.

    // Controls how the moving block is moved
    ActionHandler* leftHandler_;
    ActionHandler* rightHandler_;
    ActionHandler* downHandler_;
    ActionHandler* rotateHandler_;

	std::queue<TetrisBoard::Move> moves_;   // The moves to update
};

typedef std::shared_ptr<Human> HumanPtr;

#endif // HUMAN_H