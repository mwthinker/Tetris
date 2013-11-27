#include "frame.h"
#include "guiitem.h"
#include "bar.h"
#include "background.h"

#include <mw/window.h>
#include <mw/color.h>

#include <functional>
#include <cmath>
#include <vector>
#include <SDL.h>
#include <SDL_opengl.h>
#include <memory>

namespace gui {

	Frame::GuiProperties::GuiProperties() {
		x_ = 0;
		y_ = 0;
		invX_ = false;
		invY_ = false;
	}

	Frame::GuiProperties::GuiProperties(int x, int y, bool invX, bool invY) {
		x_ = x;
		y_ = y;
		invX_ = invX;
		invY_ = invY;
	}

	Frame::Frame(SDL_Window* window) : background_(nullptr), window_(window) {
	}

	void Frame::unfocusAll() {
		for (Item_& item : items_) {
			item.first->setFocus(false);
		}
	}

	void Frame::setBackground(BackgroundPtr background) {
		background_ = background;
	}

	BackgroundPtr Frame::getBackground() const {
		return background_;
	}

	void Frame::addBar(BarPtr bar) {
		GuiProperties properties;

		// Sets the position.
		switch (bar->getType()) {
		case Bar::UP:
			properties.y_ = bar->getSize();
			properties.invY_ = true;
			break;
		case Bar::DOWN:
			break;
		case Bar::LEFT:
			break;
		case Bar::RIGHT:
			properties.invX_ = true;
			properties.x_ = bar->getSize();
			break;
		}

		BarItem_ item(bar,properties);
		barItems_.push_back(item);
	}

	void Frame::add(GuiItemPtr guiItem, int x, int y, bool invX, bool invY) {
		GuiProperties p(x,y,invX,invY);

		Item_ item(guiItem,GuiProperties(x,y,invX,invY));
		items_.push_back(item);
	}

	void Frame::eventUpdate(const SDL_Event& windowEvent) {
		int x, y;
		mousePosition(x,y);
		int w, h;
		windowSize(w,h);

		for (Item_& item : items_) {
			if (item.first->isVisible()) {
				int xTemp = item.second.x_;
				int yTemp = item.second.y_;
				if (item.second.invX_) {
					xTemp = w - item.second.x_ - item.first->getWidth();
				}
				if (item.second.invY_) {
					yTemp = h - item.second.y_ - item.first->getHeight();
				}

				item.first->eventUpdate(windowEvent,x - xTemp,y - yTemp);
				item.first->updateSdlEventHandler(windowEvent);
			}
		}
	}

	void Frame::draw() {
		int w, h;
		windowSize(w,h);

		// Draw background?
		if (background_ != 0) {
			background_->draw(w,h);
		}

		// Draw all visible Bars.
		for (BarItem_& item : barItems_) {			
			GuiProperties& p = item.second;
			glPushMatrix();
			int x = p.x_;
			if (p.invX_) {
				x = w - p.x_;
			}
			int y = p.y_;
			if (p.invY_) {
				y = h - p.y_;
			}
			glTranslated(x,y,0.0);
			switch (item.first->getType()) {
			case Bar::UP:
				// Fall through.
			case Bar::DOWN:
				item.first->draw(w,item.first->getSize());
				break;
			case Bar::LEFT:
				// Fall through.
			case Bar::RIGHT:
				item.first->draw(item.first->getSize(),h);
				break;
			}				
			glPopMatrix();
		}

		// Draw all visible Gui Items.
		for (Item_& item : items_) {
			if (item.first->isVisible()) {
				GuiProperties& p = item.second;
				glPushMatrix();
				int x = p.x_;
				if (p.invX_) {
					x = w - p.x_ - item.first->getWidth();
				}
				int y = p.y_;
				if (p.invY_) {
					y = h - p.y_ - item.first->getHeight();
				}
				glTranslated(x,y,0.0);
				item.first->draw();
				glPopMatrix();
			}
		}
	}

	void Frame::windowSize(int& width, int& height) const {
		SDL_GetWindowSize(window_, &width, &height);
	}

	void Frame::mousePosition(int& x, int& y) const {
		int tmpX;
		int tmpY;
		SDL_GetMouseState(&tmpX, &tmpY);
		int width, height;
		windowSize(width, height);
		x = tmpX;
		y = height - tmpY;
	}

} // Namespace gui.
