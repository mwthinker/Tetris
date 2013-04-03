#ifndef INPUTDEVICE_H
#define INPUTDEVICE_H

#include <queue>
#include <SDL.h>

template <typename Event>
class InputDevice {
public:
	virtual ~InputDevice() {
	}

    virtual void eventUpdate(const SDL_Event& windowEvent) = 0;

    bool pollEvent(Event& newEvent) {
        if (events_.empty()) {
            return false;
        }
        newEvent = events_.front();
        events_.pop();
        return true;
    }
protected:
    void pushEvent(Event gameEvent) {
        events_.push(gameEvent);
    }

private:
    std::queue<Event> events_;
};

#endif // INPUTDEVICE_H
