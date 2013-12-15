#include "tbutton.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <SDL_opengl.h>

namespace gui {

	TButton::TButton() {
		focused_ = mw::Color(.8, .1, 0, .3);
		textColor_ = mw::Color(1, 0.1, 0.1);
		onHover_ = mw::Color(.6, .1, .1);
		notHover_ = mw::Color(.4, .0, .0, .0);
		insideDownHover_ = mw::Color(.8, .0, 0, .7);
	}

	TButton::TButton(std::string text, mw::FontPtr font) : text_(text, font) {
		focused_ = mw::Color(.8, .1, 0, .3);
		textColor_ = mw::Color(1, 0.1, 0.1);
		onHover_ = mw::Color(.6, .1, .1);
		notHover_ = mw::Color(.4, .0, .0, .0);
		insideDownHover_ = mw::Color(.8, .0, 0, .7);
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
		glDisable(GL_BLEND);

		textColor_.glColor4d();
		glPushMatrix();
		glTranslatef(x, y, 0);
		text_.draw();
		glPopMatrix();
	}

} // Namespace gui.
