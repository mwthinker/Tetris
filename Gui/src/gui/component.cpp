#include "component.h"

namespace gui {

	void Component::draw(Uint32 deltaTime) {
		// Draw panel background.
		backgroundColor_.glColor4d();
		glPushMatrix();
		Dimension dim = getSize();
		glScaled(dim.width_, dim.height_, 1);
		glTranslated(0.5, 0.5, 0);
		background_.draw();
		glPopMatrix();
		drawBorder();
	}

} // Namespace gui.
