#include "button.h"

#include <functional>
#include <string>

namespace gui {

	Button::Button() {
		pushed_ = false;
		mouseDown_ = false;
		mouseInside_ = false;
		setWidth(50);
		setHeight(50);
	}

	void Button::eventUpdate(const SDL_Event& windowEvent, int x, int y) {		
		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:
			if (isInside(x,y)) {
				mouseInside_ = true;
			} else {
				mouseInside_ = false;
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				{
					if (isInside(x,y)) {
						mouseInside_ = true;
						pushed_ = true;
						setFocus(true);
					} else {
						mouseInside_ = false;
						pushed_ = false;
						setFocus(false);
					}
					mouseDown_ = true;
				}
				break;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				if (isInside(x,y)) {
					mouseInside_ = true;
				} else {
					mouseInside_ = false;
				}

				if (mouseInside_ && hasFocus() && pushed_) {
					setFocus(true);
					onClick();
				}

				mouseDown_ = false;
				pushed_ = false;
				break;
			}
			break;
		default:
			break;
		}
	}

	bool Button::isMouseInside() const {
		return mouseInside_;
	}

	bool Button::isPushed() const {
		return pushed_;
	}

	bool Button::isMouseDown() const {
		return mouseDown_;
	}

	void Button::click() {
		onClick();
	}	

	void Button::excecute() {
		click();
	}

} // Namespace gui.