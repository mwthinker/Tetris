#include "label.h"

#include <mw/font.h>
#include <mw/text.h>

namespace gui {

	Label::Label(std::string text, const mw::FontPtr& font) {
		text_ = mw::Text(text, font);
		if (text_.getWidth() > 1 && text_.getWidth() > 1) {
			setPreferredSize((float) text_.getWidth(), (float) text_.getHeight());
		}
		init();
	}

	Label::Label(mw::Text text) {
		text_ = text;
		if (text_.getWidth() > 1 && text_.getWidth() > 1) {
			setPreferredSize((float) text_.getWidth(), (float) text_.getHeight());
		}
		init();
	}
	
	void Label::draw(float deltaTime) {
		Component::draw(deltaTime);

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
		color_.glColor4d();
		glPushMatrix();
		glTranslatef(x, y, 0);
		text_.draw();
		glPopMatrix();
	}

	void Label::setFont(const mw::FontPtr& font) {
		text_ = mw::Text(text_.getText(), font);
	}

	std::string Label::getText() const {
		return text_.getText();
	}

	void Label::setText(std::string text) {
		text_.setText(text);
		if (text_.getWidth() > 1 && text_.getWidth() > 1) {
			setPreferredSize((float) text_.getWidth(), (float) text_.getHeight());
		}
	}

	Label::HorizontalAlignment Label::getHorizontalAlignment() const {
		return hTextAlignment_;
	}
	
	void Label::setHorizontalAlignment(HorizontalAlignment alignment) {
		hTextAlignment_ = alignment;
	}

	Label::VerticalAlignment Label::getVerticalAlignment() const {
		return vTextAlignment_;
	}

	void Label::setVerticalAlignment(VerticalAlignment alignment) {
		vTextAlignment_ = alignment;
	}
		
	void Label::setTextColor(const mw::Color& color) {
		color_ = color;
	}

	void Label::init() {
		color_ = mw::Color(0, 0, 0);
		hTextAlignment_ = HCENTER;
		vTextAlignment_ = VCENTER;		
	}

} // Namespace gui.