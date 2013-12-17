#ifndef FRAME_H
#define FRAME_H

#include "panel.h"

#include <mw/window.h>
#include <mw/color.h>
#include <mw/signal.h>

#include <SDL.h>

#include <string>
#include <queue>

namespace gui {	

	class Frame;
	using WindowListener = mw::Signal<Frame*, const SDL_Event&>;

	// TextField, Checkbox, CheckboxGroup (radio buttons), List, and Choice
	class Frame : public mw::Window, public Panel {
	public:
		Frame();
		Frame(int width, int height, bool resizeable = true, std::string title = "Frame", std::string icon = "");

		mw::signals::Connection addWindowListener(const WindowListener::Callback& callback);

		void setDefaultClosing(bool defaultClosing);

		bool isDefaultClosing() const;

	private:
		// Override mw::Window.
		void update(Uint32 deltaTime) override;

		// Override mw::Window.
		void eventUpdate(const SDL_Event& windowEvent) override;

		void resize();

		void init();

		std::queue<SDL_Event> eventQueue_;
		WindowListener windowListener_;
		bool defaultClosing_;
	};

} // Namespace gui.

#endif // FRAME_H
