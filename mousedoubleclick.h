#ifndef MOUSEDOUBLECLICK_H
#define MOUSEDOUBLECLICK_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <queue>
#include <vector>

#include "gui/guiitem.h"
#include "menuevent.h"

class MouseDoubleClick : public gui::GuiItem<MenuEvent> {
public:
	MouseDoubleClick(MenuEvent fullScreenEvent) {
		fullScreenEvent_ = fullScreenEvent;
		time_ = 0;
		nbrOfPressed_ = 0;
	}

	void graphicUpdate(Uint32 deltaTime) {
		time_ += deltaTime;
		if (nbrOfPressed_ >= 2) {
			nbrOfPressed_ = 0;
			push(fullScreenEvent_);
		}
		if (time_ > 500) {
			if (nbrOfPressed_ > 0) {
				nbrOfPressed_ -= 1;
			}
			time_ = 0;
		}
	}

	void eventUpdate(const SDL_Event& windowEvent) {		
		switch (windowEvent.type) {
		case SDL_MOUSEMOTION:			
			break;
		case SDL_MOUSEBUTTONDOWN:
			switch (windowEvent.button.button) {
			case SDL_BUTTON_LEFT:
				++nbrOfPressed_;
				std::cout << "\n" << nbrOfPressed_;
				break;					
			}			
			break;
		}
	}
private:
	Uint32 time_;
	int nbrOfPressed_;
	MenuEvent fullScreenEvent_;
};

#endif // MOUSEDOUBLECLICK_H