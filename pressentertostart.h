#ifndef PRESSENTERTOSTART_H
#define PRESSENTERTOSTART_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <queue>
#include <vector>

#include "gui/guiitem.h"
#include "menuevent.h"

class PressEnterToStart : public gui::GuiItem<MenuEvent> {
public:
	PressEnterToStart(mw::FontPtr font, MenuEvent menuEvent) {
		menuEvent_ = menuEvent;
		active_ = false;
		text_ = mw::Text("Press Enter to start!",font);
	}

	void setActive(bool active) {
		active_ = active;
	}
private:
	void graphicUpdate(Uint32 deltaTime) {
		//int w, h;
		//windowSize(w,h);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glPushMatrix();
		glScaled(20,20,0);
		glColor4d(0.1,0,0,1);
		glBegin(GL_QUADS);
		glVertex2d(-0.5,-0.5);
		glVertex2d( 0.5,-0.5);
		glVertex2d( 0.5, 0.5);
		glVertex2d(-0.5, 0.5);
		glEnd();

		glColor4d(0.8,0,0,1);
		glTranslated(-text_.getHeight()*0.5,-text_.getWidth()*0.5,0);
		
		glPopMatrix();

		glDisable(GL_BLEND);
	}

	void eventUpdate(const SDL_Event& windowEvent) {		
		switch (windowEvent.type) {
		case SDL_KEYDOWN:            
			case SDLK_RETURN:
				if (active_) {
					push(menuEvent_);
					active_ = false;
				}
				break;			
			break;
		}
	}

	MenuEvent menuEvent_;
	mw::Text text_;
	bool active_;
};

#endif // PRESSENTERTOSTART_H