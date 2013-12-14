#include "tbutton.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <SDL_opengl.h>

namespace gui {

	TButton::TButton(std::string text, mw::FontPtr font) : text_(text, font) {
		focused_ = mw::Color(0.8, 0.8, 0.8);
		textColor_ = mw::Color(1.0, 0.1, 0.1);
		onHover_ = mw::Color(0.6, 0.1, 0.1);
		notHover_ = mw::Color(0.4, 0.0, 0.0);
		insideDownHover_ = mw::Color(0.5, 0.0, 0.5);
	}

	TButton::TButton(std::string text, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
		mw::Color notHover, mw::Color insideDownHover) : text_(text, font) {
		textColor_ = textColor;
		focused_ = focused;
		onHover_ = onHover;
		notHover_ = notHover;
		insideDownHover_ = insideDownHover;
	}

	void TButton::draw(float deltaTime) {
		Dimension dim = getSize();

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
		glVertex3f(dim.width_*1.0f, 0.0f, 0.0f);
		glVertex3f(dim.width_*1.0f, dim.height_*1.0f, 0.0f);
		glVertex3f(0.0f, dim.height_*1.0f, 0.0f);
		glEnd();

		if (hasFocus()) {
			focused_.glColor4d();
			glBegin(GL_QUADS);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(dim.width_*1.0f, 0.0f, 0.0f);
			glVertex3f(dim.width_*1.0f, dim.height_*1.0f, 0.0f);
			glVertex3f(0.0f, dim.height_*1.0f, 0.0f);
			glEnd();
		}

		textColor_.glColor4d();
		text_.draw();

		glDisable(GL_BLEND);
	}

} // Namespace gui.
