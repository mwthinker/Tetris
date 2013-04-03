#ifndef GUIITEM_H
#define GUIITEM_H

#include <SDL.h>
#include <queue>

namespace gui {

template <typename GuiEvent>
class GuiItem {
public:
	GuiItem() {
		visible_ = true;
		active_ = true;
	}

	virtual ~GuiItem() {
	}

	bool isActive() const {
		return active_;
	}

	void setActive(bool active) {
		return active_;
	}

	bool isVisible() const {
		return visible_;
	}

	void setVisible(bool visible) {
		visible_ = visible;
	}	

	virtual void physicUpdate(Uint32 deltaTime) {}
	virtual void graphicUpdate(Uint32 deltaTime) {};
	virtual void eventUpdate(const SDL_Event& windowEvent) {};

	bool pollEvent(GuiEvent& newEvent) {
        if (events_.empty()) {
            return false;
        }
        newEvent = events_.front();
        events_.pop();
        return true;
    }
protected:
	void push(const GuiEvent& guiEvent) {
		events_.push(guiEvent);
	}

	// Saves the current position for the mouse in x and y.
	void mousePosition(int& x, int& y) {
		int tmpX;
		int tmpY;
		SDL_GetMouseState(&tmpX, &tmpY);
		int w,h;
		windowSize(w,h);

		x = tmpX;
		y = h - tmpY;  // Transform the sdl postion to the reference system of the opengl graphic.
	}

	void windowSize(int& width, int& height) {
		SDL_Surface* surface = SDL_GetVideoSurface();
		width = surface->w;
		height = surface->h;
	}
private:
	//template <class GuiEvent>
	//template <class>
	//friend class Gui; // In order for gui to access setFixProportions but hide it from classes inherited from this one.

	std::queue<GuiEvent> events_;

	bool visible_;
	bool active_;
};

} // namespace gui

#endif // GUIITEM_H
