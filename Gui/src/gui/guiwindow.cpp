#include "guiwindow.h"
#include "flowlayout.h"
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

	namespace {

		TButton* createButton(std::string text) {
			mw::Color textColor(1, 1, 1);
			mw::Color textButtonColor(1, .1, .1);
			mw::Color focusColo(.8, .1, 0, .3);
			mw::Color onHoverColor(.6, .1, .1);
			mw::Color notHoverColor(.4, .0, .0, .0);
			mw::Color pushedColor(.8, .0, 0, .7);
			return nullptr;
			//return new TButton(text, size, fontDefault, textButtonColor_, focusColor_, onHoverColor_, notHoverColor_, pushedColor_);			
		}

	}

	Frame::Frame() : mw::Window(520, 640, "MWetris", "images/tetris.bmp") {
		// Init the opengl settings.
		resize();
		add(new TButton);

		setLayout(new FlowLayout());
		setBackground(mw::Color(0, 1, 0));
		setSize((float) getWidth(), (float) getHeight());
		setPreferredSize((float) getWidth(), (float) getHeight());
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
							setPreferredSize((float) sdlEvent.window.data1, (float) sdlEvent.window.data2);
							setSize((float) sdlEvent.window.data1, (float) sdlEvent.window.data2);
							setLocation(0, 0);
							resize();
							break;
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
					sdlEvent.motion.y = getHeight() - sdlEvent.motion.y;
					handleMouse(sdlEvent);
					break;
				case SDL_MOUSEBUTTONDOWN:
					// Fall through!
				case SDL_MOUSEBUTTONUP:
					sdlEvent.button.y = getHeight() - sdlEvent.motion.y;
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

} // Namespace gui.
