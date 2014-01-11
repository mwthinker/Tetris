#include "component.h"

namespace gui {

	void Component::draw(Uint32 deltaTime) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// Draw panel background.
		backgroundColor_.glColor4d();

		Dimension dim = getSize();

		if (background_) {
			glEnable(GL_TEXTURE_2D);
			background_->bind();

			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex2f(0, 0);
			glTexCoord2f(1, 0);
			glVertex2f(dim.width_ * 1.f, 0.f);
			glTexCoord2f(1, 1);
			glVertex2f(dim.width_ * 1.f, dim.height_ * 1.0f);
			glTexCoord2f(0, 1);
			glVertex2f(0, dim.height_ * 1.f);
			glEnd();

			glDisable(GL_TEXTURE_2D);
		} else {
			glBegin(GL_QUADS);
			glVertex2f(0, 0);
			glVertex2f(dim.width_ * 1.f, 0.f);
			glVertex2f(dim.width_ * 1.f, dim.height_ * 1.0f);
			glVertex2f(0, dim.height_ * 1.f);
			glEnd();
		}
		drawBorder();

		glDisable(GL_BLEND);
	}

} // Namespace gui.
