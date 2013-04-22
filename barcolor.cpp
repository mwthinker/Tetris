#include "barcolor.h"

#include <mw/color.h>

#include <SDL_opengl.h>

namespace gui {

	BarColor::BarColor(Bar::Type type, int size, mw::Color color) : Bar(type,size), color_(color) {
	}

	// Override View
	void BarColor::draw(int width, int height) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		color_.glColor4d();
		glBegin(GL_QUADS);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(width*1.0f, 0.0f, 0.0f);
		glVertex3f(width*1.0f, height*1.0f, 0.0f);
		glVertex3f(0.0f, height*1.0f, 0.0f);
		glEnd();

		glDisable(GL_BLEND);
	}

} // Namespace gui.