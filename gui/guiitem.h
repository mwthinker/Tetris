#ifndef GUIITEM_H
#define GUIITEM_H

#include "background.h"

#include <functional>
#include <cmath>
#include <vector>

#include <memory>
#include <SDL.h>
#include <SDL_opengl.h>

namespace gui {

	class GuiProperties {
	public:
		GuiProperties() {
			x_ = 0;
			y_ = 0;
			visible_ = true;
			invX_ = false;
			invY_ = false;
		}

		GuiProperties(int x, int y, bool invX, bool invY) {
			x_ = x;
			y_ = y;
			visible_ = true;
			invX_ = invX;
			invY_ = invY;
		}

		int x_, y_;  // Global position.
		bool visible_;
		bool invX_, invY_;  // Invert the x and y axis if true.	
	};

	class GuiItem {
	public:
		friend class MultiFrame;
		friend class Group;

		GuiItem() {
			focus_ = false;
			width_ = 0;
			height_ = 0;
		}

		virtual ~GuiItem() {
		}

		virtual void eventUpdate(const SDL_Event& windowEvent, int x, int y) {
		}

		virtual void draw() {
		}	

		int getWidth() const {
			return width_;
		}

		int getHeight() const {
			return height_;
		}

		void setFocus(bool focus) {
			focus_ = focus;
		}

		bool hasFocus() const {
			return focus_;
		}

		static void windowSize(int& width, int& height) {
			SDL_Surface* surface = SDL_GetVideoSurface();
			width = surface->w;
			height = surface->h;
		}

		void setWidth(int width) {
			width_ = width;
		}

		void setHeight(int height) {
			height_ = height;
		}

		bool isInside(int x, int y) const {
			return x >= 0 && x < width_ &&  y >= 0 && y <= height_;
		}

	protected:
		virtual void excecute() {
		}
	private:
		int width_, height_;
		bool focus_;
	};

	typedef std::shared_ptr<GuiItem> GuiItemPtr;

} // Namespace gui.

#endif // GUIITEM_H
