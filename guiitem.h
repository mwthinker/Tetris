#ifndef GUIITEM_H
#define GUIITEM_H

#include "background.h"

#include <functional>
#include <cmath>
#include <vector>

#include <mw/signal.h>

#include <memory>
#include <SDL.h>
#include <SDL_opengl.h>

namespace gui {

	class GuiItem {
	public:
		friend class MultiFrame;
		friend class Frame;
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

		mw::signals::Connection addOnClickListener(std::function<void(GuiItem*)> onClick) {
			return onClick_.connect(onClick);
		}

		mw::signals::Connection addSdlEventListener(std::function<void(GuiItem*,const SDL_Event&)> sdlEvent) {
			return sdlEventHandler_.connect(sdlEvent);
		}

	protected:
		virtual void excecute() {
		}

		void onClick() {
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
	};

	typedef std::shared_ptr<GuiItem> GuiItemPtr;

} // Namespace gui.

#endif // GUIITEM_H
