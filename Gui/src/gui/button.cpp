#include "button.h"

#include <functional>
#include <string>

namespace gui {

	Button::Button() {
		init();
	}

	Button::Button(std::string text, const mw::FontPtr& font) : text_(text , font) {
		init();
	}

	Button::Button(mw::Text text) : text_(text) {
		init();
	}

	void Button::draw(float deltaTime) {
		Component::draw(deltaTime);
		drawLabel();
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
						if (mouseDown_) {
							doAction();
							mouseDown_ = false;
							pushed_ = false;
						}
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

	void Button::setTextColor(const mw::Color& textColor) {
		textColor_ = textColor;
	}

	void Button::drawLabel() {
		Dimension dim = getSize();
		float x = 0.0;
		switch (hTextAlignment_) {
			case HorizontalAlignment::LEFT:
				x = 0;
				break;
			case HorizontalAlignment::HCENTER:
				x = dim.width_ * 0.5f - (float) text_.getWidth() * 0.5f;
				break;
			case HorizontalAlignment::RIGHT:
				x = dim.width_ - (float) text_.getWidth();
				break;
		}
		float y = 0.0;
		switch (vTextAlignment_) {
			case VerticalAlignment::BOTTOM:
				y = 0;
				break;
			case VerticalAlignment::VCENTER:
				y = dim.height_ * 0.5f - (float) text_.getHeight() * 0.5f;
				break;
			case VerticalAlignment::TOP:
				y = dim.height_ - (float) text_.getHeight();
				break;
		}
		textColor_.glColor4d();
		glPushMatrix();
		glTranslatef(x, y, 0);
		text_.draw();
		glPopMatrix();
	}

	void Button::preferedSizeFitText() {
		if (text_.getWidth() > 1 && text_.getHeight() > 1) {
			setPreferredSize(2 + (float) text_.getWidth(), 2 + (float) text_.getHeight());
		}
	}
	
} // Namespace gui.
