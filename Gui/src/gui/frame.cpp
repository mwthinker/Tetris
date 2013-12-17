#include "frame.h"
#include "borderlayout.h"

#include <mw/font.h>
#include <mw/window.h>
#include <mw/exception.h>

#include <string>

namespace gui {	

	Frame::Frame() : mw::Window(512, 512, true, "Frame", "") {
		init();
	}

	Frame::Frame(int width, int height, bool resizeable, std::string title, std::string icon) : mw::Window(width, height, resizeable, title, icon) {
		init();
	}

	mw::signals::Connection Frame::addWindowListener(const WindowListener::Callback& callback) {
		return windowListener_.connect(callback);
	}

	void Frame::setDefaultClosing(bool defaultClosing) {
		defaultClosing_ = true;
	}

	bool Frame::isDefaultClosing() const {
		return defaultClosing_;
	}

	void Frame::resize() {
		setPreferredSize((float) getWidth(), (float) getHeight());
		setSize((float) getWidth(), (float) getHeight());
		setLocation(0, 0);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glViewport(0, 0, getWidth(), getHeight());
		glOrtho(0, getWidth(), 0, getHeight(), -1, 1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	// Override mw::Window
	void Frame::update(Uint32 deltaTime) {
		// Perform non critical event updates.
		while (!eventQueue_.empty()) {
			SDL_Event sdlEvent = eventQueue_.front();
			eventQueue_.pop();

			switch (sdlEvent.type) {
				case SDL_WINDOWEVENT:
					if (sdlEvent.window.windowID == SDL_GetWindowID(getSdlWindow())) {
						windowListener_(this, sdlEvent);
						switch (sdlEvent.window.event) {
							case SDL_WINDOWEVENT_RESIZED:
								resize();
								break;
							case SDL_WINDOWEVENT_LEAVE:
								mouseMotionLeave();
								break;
							case SDL_WINDOWEVENT_CLOSE:
								if (defaultClosing_) {
									quit();
								}
							default:
								break;
						}
					}
					break;
				case SDL_MOUSEMOTION:
					if (sdlEvent.motion.windowID == SDL_GetWindowID(getSdlWindow())) {
						// Reverse y-axis.
						sdlEvent.motion.yrel *= -1;
						sdlEvent.motion.y = getHeight() - sdlEvent.motion.y - 1;
						handleMouse(sdlEvent);
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					// Fall through!
				case SDL_MOUSEBUTTONUP:
					if (sdlEvent.button.windowID == SDL_GetWindowID(getSdlWindow())) {
						sdlEvent.button.y = getHeight() - sdlEvent.motion.y - 1;
						handleMouse(sdlEvent);
					}
					break;
				case SDL_KEYDOWN:
					// Fall through.
				case SDL_KEYUP:
					if (sdlEvent.key.windowID == SDL_GetWindowID(getSdlWindow())) {
						handleKeyboard(sdlEvent);
						switch (sdlEvent.key.keysym.sym) {
							case SDLK_ESCAPE:
								if (defaultClosing_) {
									quit();
								}
								break;
							default:
								break;
						}
					}
					break;
				default:
					break;
			}
		}
		draw(deltaTime / 1000.f);
	}

	// Override mw::Window
	void Frame::eventUpdate(const SDL_Event& windowEvent) {
		eventQueue_.push(windowEvent);
	}

	void Frame::init() {
		// The default frame for Frame.
		setLayout(new BorderLayout());

		// Init the opengl settings.
		resize();

		setBackgroundColor(mw::Color(1, 1, 1));
		setSize((float) getWidth(), (float) getHeight());
		setPreferredSize((float) getWidth(), (float) getHeight());
		defaultClosing_ = false;
	}

} // Namespace gui.
