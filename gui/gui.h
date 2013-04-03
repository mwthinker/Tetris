#ifndef GUI_H
#define GUI_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <queue>
#include <vector>
#include <utility>

#include "guiitem.h"

namespace gui {

template <typename GuiEvent>
class Gui {
public:
	Gui() {
		activeFrame_ = 0;		
	}

	void add(int frame, GuiItem<GuiEvent>* guiItem) {
		guiItems_.push_back(Pair(frame,guiItem));		
	}

	void physicUpdate(Uint32 deltaTime) {
		for (auto it = guiItems_.begin(); it != guiItems_.end(); ++it) {
			GuiItem<GuiEvent>* guiItem = it->second;
			int frame = it->first;
			if (frame == activeFrame_ && guiItem->isActive()) {
				guiItem->physicUpdate(deltaTime);
			}
		}
	}

	void graphicUpdate(Uint32 deltaTime) {
		glPushMatrix();

		for (auto it = guiItems_.begin(); it != guiItems_.end(); ++it) {
			GuiItem<GuiEvent>* guiItem = it->second;
			int frame = it->first;
			if (frame == activeFrame_ && guiItem->isVisible()) {
				guiItem->graphicUpdate(deltaTime);
			}
		}

		glPopMatrix();
	}

	void eventUpdate(const SDL_Event& windowEvent) {
		for (auto it = guiItems_.begin(); it != guiItems_.end(); ++it) {
			GuiItem<GuiEvent>* guiItem = it->second;
			int frame = it->first;
			if (frame == activeFrame_ && guiItem->isActive()) {
				guiItem->eventUpdate(windowEvent);
				GuiEvent guiEvent;
				while (guiItem->pollEvent(guiEvent)) {
					guiEvents_.push(guiEvent);
				}
			}
		}
	}

	void pushEvent(const GuiEvent& guiEvent) {
		guiEvents_.push(guiEvent);
	}

	bool pollEvent(GuiEvent& newEvent) {
        if (guiEvents_.empty()) {
            return false;
        }
        newEvent = guiEvents_.front();
        guiEvents_.pop();
        return true;
    }

	void setActiveFrame(int frame) {
		activeFrame_ = frame;
	}

	int getActiveFrame() const {
		return activeFrame_;
	}

	/*int getWidth() const  {
		return width_;
	}

	int getHeight() const  {
		return height_;
	}*/

	//typedef boost::shared_ptr< GuiItem<GuiEvent> > GuiItemPtr;
private:
	typedef std::pair<int,GuiItem<GuiEvent>*> Pair;

	void realWindowSize(int& width, int& height) {
		SDL_Surface* surface = SDL_GetVideoSurface();
		width = surface->w;
		height = surface->h;
	}

	std::vector<Pair> guiItems_;
	std::queue<GuiEvent> guiEvents_;

	int activeFrame_;
};

} // namespace gui

#endif // GUI_H