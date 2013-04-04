#ifndef IMAGEBACKGROUND_H
#define IMAGEBACKGROUND_H

#include "background.h"

#include <mw/sprite.h>

#include <SDL_opengl.h>

namespace gui {

	class ImageBackground : public Background {
	public:
		ImageBackground(mw::Sprite sprite) {
			sprite_ = sprite;
		}

		void draw(int width, int height) override {
			glPushMatrix();
			glColor3d(1,1,1);
			glScaled(width,height,1);
			glTranslated(0.5,0.5,0);
			sprite_.draw();
			glPopMatrix();
		}

	private:
		mw::Sprite sprite_;
	};

} // Namespace gui.

#endif // IMAGEBACKGROUND_H
