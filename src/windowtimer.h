#ifndef WINDOWTIMER_H
#define WINDOWTIMER_H

#include <SDL_events.h>
#include <SDL_timer.h>

class WindowTimer {
protected:
	WindowTimer();

	virtual ~WindowTimer();

	void addTimerMS(unsigned int ms);

	void removeLastTimer();

private:
	static Uint32 addTimer(Uint32 interval, void* sdlTimerId);

	SDL_TimerID lastTimerId_;
};

#endif // WINDOWTIMER_H
