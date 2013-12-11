#ifndef WIDGET_H
#define WIDGET_H

#include <mw/signal.h>

#include <SDL.h>

#include <functional>
#include <vector>

namespace gui {

	class Widget {
	public:
		Widget() {
			window_ = nullptr;
			focus_ = false;
			visible_ = true;
			width_ = 0;
			height_ = 0;
		}

		virtual ~Widget() {
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
			if (!visible_) {
				focus_ = false;
			}
		}

		bool isVisible() const {
			return visible_;
		}

		void setWidth(int width) {
			width_ = width;
		}

		void setHeight(int height) {
			height_ = height;
		}

		mw::signals::Connection addOnClickListener(std::function<void(Widget*)> onClick) {
			return onClick_.connect(onClick);
		}

		mw::signals::Connection addSdlEventListener(std::function<void(Widget*, const SDL_Event&)> sdlEvent) {
			return sdlEventHandler_.connect(sdlEvent);
		}

		void click() {
			onClick_(this);
		}

		SDL_Window* getWindow() const {
			return window_;
		}

	private:
		SDL_Window* window_;

		// Updates the sdlEventhander.
		void updateSdlEventHandler(const SDL_Event& windowEvent) {
			sdlEventHandler_(this, windowEvent);
		}

		mw::Signal<Widget*> onClick_;
		mw::Signal<Widget*, const SDL_Event&> sdlEventHandler_;

		int width_, height_;
		int x_, y_;

		bool focus_;
		bool visible_;
	};

} // Namespace gui.

#endif // WIDGET_H
