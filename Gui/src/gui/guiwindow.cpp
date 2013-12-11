#include "guiwindow.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

namespace gui {

	Frame::Frame() : mw::Window(520, 640, "MWetris", "images/tetris.bmp") {
		// Init the opengl settings.
		resize();
	}

	void Frame::resize() {
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
			SDL_Event windowEvent = eventQueue_.front();
			eventQueue_.pop();

			switch (windowEvent.type) {
				case SDL_KEYDOWN:
					switch (windowEvent.key.keysym.sym) {
						case SDLK_ESCAPE:
							quit();
							break;
						case SDLK_F11:
							mw::Window::setFullScreen(!mw::Window::isFullScreen());
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}
	}

	// Override mw::Window
	void Frame::eventUpdate(const SDL_Event& windowEvent) {
		if (windowEvent.type == SDL_WINDOWEVENT) {
			switch (windowEvent.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					//resize(windowEvent.window.data1, windowEvent.window.data2);
					break;
				default:
					break;
			}
		}
		eventQueue_.push(windowEvent);
	}

} // Namespace gui.
