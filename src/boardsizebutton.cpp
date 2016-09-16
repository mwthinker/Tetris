#include "boardsizebutton.h"

#include <mw/opengl.h>

BoardSizeButton::BoardSizeButton(int minX, int minY, int maxX, int maxY, const mw::Sprite& icon) {

}


void BoardSizeButton::draw(const gui::Graphic& graphic, double deltaTime) {
	gui::Dimension size = getSize();
	if (pushed_) {
		graphic.drawSprite(iconDown_, 0, 0, size.width_, size.height_);		
	} else {
		graphic.drawSprite(icon_, 0, 0, size.width_, size.height_);
	}
}

void BoardSizeButton::handleMouse(const SDL_Event& mouseEvent) {
	switch (mouseEvent.type) {
		case SDL_MOUSEMOTION:
			mouseInside_ = true;
			break;
		case SDL_MOUSEBUTTONDOWN:
			mouseInside_ = true;
			break;
		case SDL_MOUSEBUTTONUP:
			switch (mouseEvent.button.button) {
				case SDL_BUTTON_LEFT:
					doAction();
					break;
				case SDL_BUTTON_RIGHT:
					break;
			}
			break;
	}
}

void BoardSizeButton::mouseMotionLeave() {
	mouseInside_ = false;
}
