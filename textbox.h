#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "guiitem.h"
#include "inputformatter.h"

#include <mw/signal.h>

#include <functional>
#include <string>

namespace gui {

	class TextBox : public GuiItem {
	public:
		TextBox(int width, int height) {
			setWidth(width);
			setHeight(height);
			inputFormatter_ = InputFormatterPtr(new InputFormatter(30));
		}

		void setInputFormatter(const InputFormatterPtr& inputFormatter) {
			inputFormatter_ = inputFormatter;
		}

		void setText(std::string str) {
			for (char a : str) {
				inputFormatter_->update(a);
			}
		}

		// Override GuiItem.
		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override {
			inside_ = isInside(x,y);

			switch (windowEvent.type) {
			case SDL_MOUSEMOTION:
				break;
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
			case SDL_MOUSEBUTTONUP:
				switch (windowEvent.button.button) {
				case SDL_BUTTON_LEFT:
					break;
				}
				break;
			case SDL_KEYDOWN: 
				{
					if (hasFocus()) {
						SDLKey sdlKey = windowEvent.key.keysym.sym;

						// Transforms a unicode character to asci, therefor it will only work as
						// intended for unicodes that has a corrensponding ascii value.
						char key = static_cast<char>(windowEvent.key.keysym.unicode);
						inputFormatter_->update(key);
						//std::cout << inputTextField_.hasInput();

						switch (sdlKey) {
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
						}
					}
				}
				break;
			default:
				break;
			};
		}

		bool isMouseInside() const {
			return inside_;
		}

		std::string getText() const {
			return inputFormatter_->getText();
		}

		int getMarkerPosition() const {
			return inputFormatter_->getMarkerPosition();
		}
		
	private:
		mw::Signal<std::string> excecute_;
		InputFormatterPtr inputFormatter_;
		bool inside_;
	};

} // Namespace gui.

#endif // TEXTBOX_H
