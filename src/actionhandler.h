#ifndef ACTIONHANDLER_H
#define ACTIONHANDLER_H

class ActionHandler {
public:
	ActionHandler();
	ActionHandler(double waitingTime, bool rebound);
	
	void update(double timeStep, bool action);
	bool doAction();
	double getWaitingTime() const;
	void setWaitingTime(double waitingTime);

	void reset();

private:
	bool action_;
	double lastTime_, waitingTime_, time_;
	bool rebound_;
	bool lastAction_;
};

#endif // ACTIONHANDLER_H
