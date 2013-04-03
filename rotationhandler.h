#ifndef ROTATIONHANDLER_H
#define ROTATIONHANDLER_H

#include "actionhandler.h"

class RotationHandler : public ActionHandler {
public:
	RotationHandler();
	void update(double timeStep, bool action);
	bool doAction();
private:
	bool action_;
	bool lastAction_;
};

#endif // ROTATIONHANDLER_H
