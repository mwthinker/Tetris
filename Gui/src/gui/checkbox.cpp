#include "checkbox.h"

#include <functional>
#include <string>

namespace gui {

	namespace {

		void doActionSelect(CheckBox* checkBox) {
			checkBox->setSelected(!checkBox->isSelected());
		}

	}

	CheckBox::CheckBox() {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = 12;
		selected_ = false;
		setPreferredSize(15, 15);
	}

	CheckBox::CheckBox(std::string text, const mw::FontPtr& font) : Button(text, font) {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = (float) (font->getCharacterSize() * 0.9);
		selected_ = false;
		setPreferredSize(boxSize_ + 3.f + (float) font->getCharacterSize(), (float) font->getCharacterSize());
	}

	CheckBox::CheckBox(mw::Text text) : Button(text) {
		addActionListener(std::bind(doActionSelect, this));
		boxSize_ = (float) (text.getCharacterSize() * 0.9);
		selected_ = false;
		setPreferredSize(boxSize_ + 3.f + (float) text.getCharacterSize(), (float) text.getCharacterSize());
	}

	void CheckBox::draw(float deltaTime) {
		Component::draw(deltaTime);
		
		textColor_.glColor4d();

		glPushMatrix();
		glTranslatef(3, 3, 0);

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
		glTranslatef(boxSize_ + 2, 0, 0);
		text_.draw();
		glPopMatrix();
	}

} // Namespace gui.
