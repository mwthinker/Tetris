#ifndef BACKGROUND_H
#define BACKGROUND_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <queue>
#include <vector>

#include "guiitem.h"

namespace gui {

template <class GuiEvent>
class Background : public GuiItem<GuiEvent> {
public:
	Background(mw::Sprite sprite) {
		sprite_ = sprite;
	}

	void graphicUpdate(Uint32 deltaTime) {
		glPushMatrix();
		glColor3d(1,1,1);
		int w,h;
		GuiItem<GuiEvent>::windowSize(w,h);

		glScaled(w,h,1);
		glTranslated(0.5,0.5,0);
		sprite_.draw();
		glPopMatrix();
	}
private:
	mw::Sprite sprite_;
};

} // namespace gui

#endif // BACKGROUND_H
