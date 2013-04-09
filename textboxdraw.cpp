#include "textboxdraw.h"
#include "color.h"

#include <mw/font.h>
#include <mw/text.h>

#include <memory>
#include <string>

namespace gui {

	TextBoxDraw::TextBoxDraw(int width, mw::FontPtr font, int characterSize,  Color textColor, Color focused, Color notFoxused) : TextBox(width,(int) (characterSize*1.2)) {
		text_ = mw::Text("",font,characterSize);
		marker_ = text_;

		focused_ = focused;
		textColor_ = textColor;
		notFoxused_ = notFoxused;
	}

	void TextBoxDraw::draw() {
		std::string str = getText();
		text_.setText(str);

		if (hasFocus()) {
			focused_ .glColor4d();
		} else {
			notFoxused_.glColor4d();
		}
		glBegin(GL_QUADS);
		glVertex2f(0.0f, 0.0f);
		glVertex2f(getWidth()*1.0f, 0.0f);
		glVertex2f(getWidth()*1.0f, getHeight()*1.0f);
		glVertex2f(0.0f, getHeight()*1.0f);
		glEnd();

		if (hasFocus()) {
			int index = getMarkerPosition();
			marker_.setText(str.substr(0,index));
			textColor_.glColor4d();
			double x = marker_.getWidth();
			glBegin(GL_LINES);
			glVertex2d(x, getHeight()*1.1);
			glVertex2d(x, -getHeight()*0.1);
			glEnd();
		}

		textColor_.glColor4d();
		text_.draw();
	}

} // Namespace gui.
