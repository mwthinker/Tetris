#ifndef HUMAN_H
#define HUMAN_H

#include "actionhandler.h"
#include "device.h"
#include "tetrisboard.h"

#include <memory>

class Human {
public:
    Human();
    ~Human();

    void update(Input input, double deltaTime, int level);
	bool pollMove(TetrisBoard::Move& move);

private:
	double calculateDownSpeed(int level) const;
	void pushMove(TetrisBoard::Move move);

    double time_; // Gametime
	double lastDownTime_; // The gametime for the last "gravity" move.

    // Controls how the moving block is moved
    ActionHandler* leftHandler_;
    ActionHandler* rightHandler_;
    ActionHandler* downHandler_;
    ActionHandler* rotateHandler_;

	std::queue<TetrisBoard::Move> moves_;   // The moves to update
};

typedef std::shared_ptr<Human> HumanPtr;

#endif // HUMAN_H
