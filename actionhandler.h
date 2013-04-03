#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

class ActionHandler {
public:
	/* Called every cycle with a timestep in time difference.
	   Action true when the "block" want to move.*/
	virtual void update(double timeStep, bool action) = 0;

	/* Returns true when the "block" is allowed to move. */
	virtual bool doAction() = 0;
	inline virtual ~ActionHandler() {}
};

#endif // ACTIONHANDLER_H
