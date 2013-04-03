#ifndef BUTTON_H
#define BUTTON_H

#include "guiitem.h"
#include "glcoordinate.h"

#include <SDL.h>

namespace gui {

template <typename GuiEvent>
class Button : public GuiItem<GuiEvent> {
public:
	Button(GuiEvent onPushEvent) {
		mode_ = MODE_NOTHING;
		onPushEvent_ = onPushEvent;
	}

	virtual ~Button() {}

	void setEventTypeOnPush(GuiEvent onPushEvent) {
		onPushEvent_ = onPushEvent;
	}

	GuiEvent getEventTypeOnPush() const {
		return onPushEvent_;
	}

	virtual void graphicUpdate(Uint32 deltaTime) = 0;

	virtual void eventUpdate(const SDL_Event& windowEvent) {
		int x, y;
		// Gets the mouse position in the graphic coordinate system.
		GuiItem<GuiEvent>::mousePosition(x,y);
		//std::cout << "\n "<< x << " " << y;

		bool inside = isInside(x,y);

		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:
			if (inside) {
				if (mode_ != MODE_MOUSEPRESSED) {
					mode_ = MODE_MOUSEOVER;
					//std::cout << "\nMODE_MOUSEOVER";
				}
			} else {
				mode_ = MODE_MOUSENOTOVER;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				if (inside) {
					mode_ = MODE_MOUSEPRESSED;
					push(onPushEvent_);
				} else {
					mode_ = MODE_NOTHING;
				}
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				mode_ = MODE_MOUSEPRESSED;
				break;
			}
			break;
		};
	}
protected:
	enum Mode {MODE_NOTHING,MODE_MOUSEOVER,MODE_MOUSENOTOVER,MODE_MOUSEPRESSED,MODE_MOUSERELEASED};

	virtual bool isInside(int x, int y) = 0;

	Mode getMode() const {
		return mode_;
	}

	void setMode(Mode mode) {
		mode_ = mode_;
	}
private:
	Mode mode_;

	GlCoordinate position_;
	GuiEvent onPushEvent_;
	int width_, height_;
};

} // namespace gui

#endif // BUTTON_H
