#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "button.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <SDL_opengl.h>

#include <memory>

namespace gui {

	class TextButton : public Button {
	public:
		TextButton(std::string text, int size, mw::FontPtr font) : text_(text,font) {
			focused_ = mw::Color(0.8, 0.8, 0.8);
			textColor_ = mw::Color(1.0,0.1,0.1);
			onHover_ = mw::Color(0.6, 0.1, 0.1);
			notHover_ = mw::Color(0.4, 0.0, 0.0);
			insideDownHover_ = mw::Color(0.5, 0.0, 0.5);		
			size_ = size;

			while (text_.getHeight() > size) {
				text_.setCharacterSize((int) (text_.getCharacterSize()-0.1));
			}

			setWidth((int) text_.getWidth());
			setHeight(size);
		}

		TextButton(std::string text, int size, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover) : text_(text,font) {
				textColor_ = textColor;
				focused_ = focused;
				onHover_ = onHover;
				notHover_ = notHover;
				insideDownHover_ = insideDownHover;
				size_ = size;

				while (text_.getHeight() > size) {
					text_.setCharacterSize((int) (text_.getCharacterSize()-0.1));
				}

				setWidth((int) text_.getWidth());
				setHeight(size);
		}

		void setText(std::string text) {
			text_.setText(text);
			setWidth((int) text_.getWidth());
		}

		std::string getText() const {
			return text_.getText();
		}

		// Override View
		void draw() override {
			int width = getWidth();
			int height = getHeight();

			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

			glDisable(GL_BLEND);
		}

		void setTextSize(int size) {
			text_.setCharacterSize(size);
		}

	private:
		mw::Color textColor_, focused_, onHover_, notHover_, insideDownHover_;
		mw::Text text_;
		int size_;
	};
	
	typedef std::shared_ptr<TextButton> TextButtonPtr;

	TextButtonPtr createTextButton(std::string text, int size, mw::FontPtr font) {
		return TextButtonPtr(new TextButton(text, size, font));
	}
	
	TextButtonPtr createTextButton(std::string text, int size, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover) {
				return TextButtonPtr(new TextButton(text, size, font, textColor, focused, onHover, notHover, insideDownHover));
	}
	
} // Namespace gui.

#endif // TEXTBUTTON_H