#include "frame.h"
#include "flowlayout.h"
#include "borderlayout.h"
#include "tbutton.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/exception.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

namespace gui {
	
	Frame::Frame() : mw::Window(512, 512, true, "Frame", "") {
		init();
	}

	Frame::Frame(int width, int height, bool resizeable, std::string title, std::string icon) : mw::Window(width, height, resizeable, title, icon) {
		init();
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
			SDL_Event sdlEvent = eventQueue_.front();
			eventQueue_.pop();

			switch (sdlEvent.type) {
				case SDL_WINDOWEVENT:
					switch (sdlEvent.window.event) {
						case SDL_WINDOWEVENT_RESIZED:
						{
							float width = (float) sdlEvent.window.data1;
							float height = (float) sdlEvent.window.data2;
							setPreferredSize(width, height);
							setSize(width, height);
							setLocation(0, 0);
							resize();
							break;
						}
						case SDL_WINDOWEVENT_LEAVE:
							mouseMotionLeave();
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					// Reverse y-axis.
					sdlEvent.motion.yrel *= -1;
					sdlEvent.motion.y = getHeight() - sdlEvent.motion.y - 1;
					handleMouse(sdlEvent);
					break;
				case SDL_MOUSEBUTTONDOWN:
					// Fall through!
				case SDL_MOUSEBUTTONUP:
					sdlEvent.button.y = getHeight() - sdlEvent.motion.y - 1;
					handleMouse(sdlEvent);
					break;
				case SDL_KEYDOWN:
					// Fall through.
				case SDL_KEYUP:
					handleKeyboard(sdlEvent);
					switch (sdlEvent.key.keysym.sym) {
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
	}

} // Namespace gui.
