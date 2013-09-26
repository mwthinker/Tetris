#include "textbox.h"
#include "inputformatter.h"

#include <mw/signal.h>

#include <string>

namespace gui {

	TextBox::TextBox(int width, int height) : inside_(false) {
		setWidth(width);
		setHeight(height);
		inputFormatter_ = InputFormatterPtr(new InputFormatter(30));
	}

	void TextBox::setInputFormatter(const InputFormatterPtr& inputFormatter) {
		inputFormatter_ = inputFormatter;
	}

	void TextBox::setText(std::string str) {
		for (char a : str) {
			inputFormatter_->update(a);
		}
	}

	// Override GuiItem.
	void TextBox::eventUpdate(const SDL_Event& windowEvent, int x, int y) {
		inside_ = isInside(x,y);

		switch (windowEvent.type) {
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				if (inside_) {
					setFocus(true);
				} else {
					setFocus(false);
				}
				break;
			}
			break;
		case SDL_KEYDOWN:
			if (hasFocus()) {
				SDLKey sdlKey = windowEvent.key.keysym.sym;

				// Transforms a unicode character to asci, therefor it will only work as
				// intended for unicodes that has a corrensponding ascii value.
				char key = static_cast<char>(windowEvent.key.keysym.unicode);
				inputFormatter_->update(key);

				switch (sdlKey) {
				case SDLK_HOME:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_HOME);
					break;
				case SDLK_END:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_END);
					break;
				case SDLK_LEFT:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_LEFT);
					break;
				case SDLK_RIGHT:
					inputFormatter_->update(InputFormatter::INPUT_MOVE_MARKER_RIGHT);
					break;
				case SDLK_BACKSPACE:
					inputFormatter_->update(InputFormatter::INPUT_ERASE_LEFT);
					break;
				case SDLK_DELETE:
					inputFormatter_->update(InputFormatter::INPUT_ERASE_RIGHT);
					break;
				default:
					// Uninteresting keys.
					break;
				}
			}
			break;
		default:
			// Uninteresting events.
			break;
		};
	}

	bool TextBox::isMouseInside() const {
		return inside_;
	}

	std::string TextBox::getText() const {
		return inputFormatter_->getText();
	}

	int TextBox::getMarkerPosition() const {
		return inputFormatter_->getMarkerPosition();
	}

} // Namespace gui.
