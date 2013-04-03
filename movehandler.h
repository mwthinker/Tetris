#ifndef MOVEHANDLER_H
#define MOVEHANDLER_H

#include "actionhandler.h"

class MoveHandler : public ActionHandler {
public:
	MoveHandler(double waitingTime);
	void update(double timeStep, bool action);
	bool doAction();
private:
	bool action_;
	double lastTime_, waitingTime_, time_;
};

#endif // MOVEHANDLER_H
