#include "button.h"

#include <functional>
#include <string>

namespace gui {

	Button::Button() {
		init();
	}

	Button::Button(std::string text, const mw::FontPtr& font) {
		text_ = mw::Text(text, font);
		init();
	}

	Button::Button(mw::Text text) {
		text_ = text;
		init();
	}

	// Get the label of this Button instance.
	std::string Button::getLabel() const {
		return text_.getText();
	}

	// Set the label of this Button instance.
	void Button::setLabel(std::string label) {
		text_.setText(label);
	}

	void Button::setFont(const mw::FontPtr& font) {
		text_ = mw::Text(text_.getText(), font);
	}

	void Button::handleKeyboard(const SDL_Event& keyEvent) {
		SDL_Keycode key = keyEvent.key.keysym.sym;
		switch (keyEvent.type) {
			case SDL_KEYDOWN:
				// Has focus and pressed enter?
				if (hasFocus() && key == SDLK_RETURN && key == SDLK_KP_ENTER) {
					doAction();
				}
				break;
			default:
				break;
		}
	}

	void Button::handleMouse(const SDL_Event& mouseEvent) {
		switch (mouseEvent.type) {
			case SDL_MOUSEMOTION:
				mouseInside_ = true;
				break;
			case SDL_MOUSEBUTTONDOWN:
				switch (mouseEvent.button.button) {
					case SDL_BUTTON_LEFT:
						pushed_ = true;
						mouseDown_ = true;
						break;
					default:
						break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (mouseEvent.button.button) {
					case SDL_BUTTON_LEFT:
						doAction();
						mouseDown_ = false;
						pushed_ = false;
						break;
					default:
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

	void Button::mouseMotionLeave() {
		mouseInside_ = false;
	}

	void Button::mouseOutsideUp() {
		mouseDown_ = false;
		pushed_ = false;
	}

	void Button::setVerticalAlignment(VerticalAlignment alignment) {
		vTextAlignment_ = alignment;
	}

	void Button::setHorizontalAlignment(HorizontalAlignment alignment) {
		hTextAlignment_ = alignment;
	}
	
} // Namespace gui.
