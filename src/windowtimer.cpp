#include "windowtimer.h"

#include <iostream>

WindowTimer::WindowTimer() : lastTimerId_(0) {
}

WindowTimer::~WindowTimer() {
	removeLastTimer();
}

void WindowTimer::addTimerMS(unsigned int ms) {
	removeLastTimer();
	lastTimerId_ = SDL_AddTimer(ms, WindowTimer::addTimer, 0); // Clean up timer?
	if (lastTimerId_ == 0) {
		std::cout << SDL_GetError() << "\n";
	}
}

void WindowTimer::removeLastTimer() {
	if (lastTimerId_ != 0) {
		SDL_RemoveTimer(lastTimerId_); // Only call this once per id.
		lastTimerId_ = 0;
	}
}

Uint32 WindowTimer::addTimer(Uint32 interval, void* sdlTimerId) {
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = nullptr;
	userevent.data2 = nullptr;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return 0;
}
