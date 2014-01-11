#include "checkbox.h"

#include <functional>
#include <string>

namespace gui {

	namespace {

		void doActionSelect(CheckBox* checkBox) {
			checkBox->setSelected(!checkBox->isSelected());
		}

	}

	CheckBox::CheckBox(int characterSize) {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = (float) characterSize;
		selected_ = false;
		setPreferredSize(boxSize_ + 2, boxSize_ + 3);
		setBorderColor(mw::Color(1, 1, 1, 0));
		setBackgroundColor(mw::Color(1, 1, 1, 0));
	}

	CheckBox::CheckBox(std::string text, const mw::FontPtr& font) : Button(text, font) {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = (float) font->getCharacterSize();
		selected_ = false;
		setPreferredSize(boxSize_ + (float) text_.getWidth() + 3, (float) text_.getHeight() + 2);
		setBorderColor(mw::Color(1, 1, 1, 0));
		setBackgroundColor(mw::Color(1, 1, 1, 0));
	}

	CheckBox::CheckBox(mw::Text text) : Button(text) {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = (float) text.getCharacterSize();
		selected_ = false;
		setPreferredSize(boxSize_ + (float) text_.getWidth() + 3, (float) text_.getHeight() + 2);
		setBorderColor(mw::Color(1, 1, 1, 0));
		setBackgroundColor(mw::Color(1, 1, 1, 0));
	}

	void CheckBox::draw(Uint32 deltaTime) {
		Component::draw(deltaTime);
		textColor_.glColor4d();

		glPushMatrix();
		glTranslatef(1, 1, 0);

		// Draw box.
		glBegin(GL_LINES);
		glVertex2f(0, 0);
		glVertex2f(boxSize_, 0);

		glVertex2f(boxSize_, 0);
		glVertex2f(boxSize_, boxSize_);

		glVertex2f(boxSize_, boxSize_);
		glVertex2f(0, boxSize_);

		glVertex2f(0, boxSize_);
		glVertex2f(0, 0);
				
		if (selected_) {
			glVertex2f(boxSize_ * 0.2f, boxSize_* 0.1f);
			glVertex2f(boxSize_ * 0.1f, boxSize_* 0.9f);

			glVertex2f(boxSize_ * 0.2f, boxSize_ * 0.1f);
			glVertex2f(boxSize_ * 0.9f, boxSize_ * 0.9f);
		}

		glEnd();

		glPopMatrix();

		glPushMatrix();
		glTranslatef(boxSize_ + 2, 1, 0);
		text_.draw();
		glPopMatrix();
	}

} // Namespace gui.
