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
		case SDL_USEREVENT:
			static_assert(GUI_CHANGE_FROM_FRAME_CODE != GUI_CHANGE_TO_FRAME_CODE,
				"In gui SDL_UserEvent error: GUI_CHANGE_FROM_FRAME_CODE must differ from GUI_CHANGE_TO_FRAME_CODE");

			// Belonging frame activated?
			if (windowEvent.user.code == GUI_CHANGE_FROM_FRAME_CODE) {
				mouseInside_ = false;
			} else if (windowEvent.user.code == GUI_CHANGE_TO_FRAME_CODE) { // Belonging frame inactivated?
                // Mouse inside of the window?
				if (SDL_GetWindowFlags(getWindow()) & SDL_WINDOW_MOUSE_FOCUS) {
                    // Check if mouse is inside.
                    mouseInside_ = isInside(x,y);
                }
			}
			break;
		case SDL_WINDOWEVENT:
			// Mouse leaves window?
			switch (windowEvent.window.event) {
				case SDL_WINDOWEVENT_LEAVE:
					mouseInside_ = false;
					break;
				default:
					break;
			}
			break;
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
					click();
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

} // Namespace gui.
