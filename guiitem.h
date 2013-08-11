#ifndef GUIITEM_H
#define GUIITEM_H

#include "guitypedefs.h"

#include <mw/signal.h>

#include <SDL.h>

#include <functional>
#include <vector>

namespace gui {

	// Identifies if the SDL_UserEvent is made by the "gui".
	// Means that the frame has changed. Random number.
	const int GUI_CHANGE_FROM_FRAME_CODE = 7219;
	const int GUI_CHANGE_TO_FRAME_CODE = 7220;

	class GuiItem {
	public:
		friend class MultiFrame;
		friend class Frame;

		GuiItem() {
			focus_ = false;
			visible_ = true;
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

		void setVisible(bool visible) {
			visible_ = visible;
		}

		bool isVisible() const {
			return visible_;
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

		mw::signals::Connection addOnClickListener(std::function<void(GuiItem*)> onClick) {
			return onClick_.connect(onClick);
		}

		mw::signals::Connection addSdlEventListener(std::function<void(GuiItem*,const SDL_Event&)> sdlEvent) {
			return sdlEventHandler_.connect(sdlEvent);
		}

		void click() {
			onClick_(this);
		}	

	private:
		void eventUpdate(const SDL_Event& windowEvent) {
			sdlEventHandler_(this, windowEvent);
		}

		mw::Signal<GuiItem*> onClick_;
		mw::Signal<GuiItem*, const SDL_Event&> sdlEventHandler_;

		int width_, height_;
		bool focus_;
		bool visible_;
	};	

} // Namespace gui.

#endif // GUIITEM_H
