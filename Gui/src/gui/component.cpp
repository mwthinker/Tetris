#include "component.h"

namespace gui {

	void Component::draw(float deltaTime) {
		mw::TexturePtr texture = getBackground();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		if (texture) {
			glEnable(GL_TEXTURE_2D);
			getBackground()->bind();
		}

		// Draw panel background.
		backgroundColor_.glColor4d();

		Dimension dim = getSize();
		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex2f(0.f, 0.f);
		glTexCoord2f(1, 0);
		glVertex2f(dim.width_*1.f, 0.f);
		glTexCoord2f(1, 1);
		glVertex2f(dim.width_*1.f, dim.height_*1.0f);
		glTexCoord2f(0, 1);
		glVertex2f(0.f, dim.height_*1.f);
		glEnd();

		if (texture) {
			glDisable(GL_TEXTURE_2D);
		}

		glDisable(GL_BLEND);
	}

} // Namespace gui.
