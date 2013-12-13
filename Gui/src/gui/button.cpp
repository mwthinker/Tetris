#include "button.h"

#include <functional>
#include <string>

namespace gui {

	Button::Button() {
		setPreferredSize(50,50);
	}

	// Get the label of this Button instance.
	std::string Button::getLabel() const {
		return text_.getText();
	}

	// Set the label of this Button instance.
	void Button::setLabel(std::string label) {
		text_.setText(label);
	}

	void Button::draw(float deltaTime) {
		Dimension dim = getSize();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		/*
		if (isPushed()) {
			if (isMouseInside()) {
				insideDownHover_.glColor4d();
			} else {
				notHover_.glColor4d();
			}
		} else if (!isMouseDown()) { // Mouse up?
			if (isMouseInside()) {
				onHover_.glColor4d();
			} else {
				notHover_.glColor4d();
			}
		} else { // Mouse down? // Should not go here!
			// But just in case.
			notHover_.glColor4d();
		}

		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(width*1.0f, 0.0f, 0.0f);
		glVertex3f(width*1.0f, height*1.0f, 0.0f);
		glVertex3f(0.0f, height*1.0f, 0.0f);
		glEnd();

		if (hasFocus()) {
			focused_.glColor4d();
			glBegin(GL_QUADS);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(width*1.0f, 0.0f, 0.0f);
			glVertex3f(width*1.0f, height*1.0f, 0.0f);
			glVertex3f(0.0f, height*1.0f, 0.0f);
			glEnd();
		}

		textColor_.glColor4d();
		text_.setText(getText());
		text_.draw();
		*/
		glDisable(GL_BLEND);
	}

	void Button::setFont(const mw::FontPtr& font) {
	}

	mw::FontPtr Button::getFont(const mw::FontPtr& font) const {
		return nullptr;
	}

	/*
	void Button::eventUpdate(const SDL_Event& windowEvent, int x, int y) {
		switch (windowEvent.type) {
		case SDL_USEREVENT:
			// Belonging frame activated?
			if (windowEvent.user.code == 1) {
				mouseInside_ = false;
			} else if (windowEvent.user.code == 2) { // Belonging frame inactivated?
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
	*/
	
} // Namespace gui.
