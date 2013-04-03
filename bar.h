#ifndef BAR_H
#define BAR_H

#include <SDL_opengl.h>

#include "gui/guiitem.h"
#include "menuevent.h"

class Bar : public gui::GuiItem<MenuEvent> {
public:
	enum Place {LEFT, RIGHT, UP, DOWN};

	Bar(Place place, int size) {
		size_ = size;
		place_ = place;
	}

	int getSize() const {
		return size_;
	}

	// TODO LEFT, RIGHT, UP
	void graphicUpdate(Uint32 deltaTime) {
		int w, h;
		windowSize(w,h);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
		switch (place_) {
		case UP:
			//glTranslated(w,h-0.5,0);
			glTranslated(0,h-size_,0);
			glScaled(w,size_,0);
			glTranslated(0.5,0.5,0);
			break;
        default:
            //TODO
            break;
		};
		glColor4d(0.8,0,0,0.1);
		glBegin(GL_QUADS);
		glVertex2d(-0.5,-0.5);
		glVertex2d( 0.5,-0.5);
		glVertex2d( 0.5, 0.5);
		glVertex2d(-0.5, 0.5);
		glEnd();
		glPopMatrix();

		glDisable(GL_BLEND);
	}
private:
	int size_;
	Place place_;
};

#endif // BAR_H
