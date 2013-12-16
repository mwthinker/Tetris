#ifndef FRAME_H
#define FRAME_H

#include "panel.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/color.h>

#include <string>
#include <queue>

namespace gui {	

	// Panel, Button, TextField, Label, Checkbox, CheckboxGroup (radio buttons), List, and Choice
	class Frame : public mw::Window, public Panel {
	public:
		Frame();
		Frame(int width, int height, bool resizeable = true, std::string title = "Frame", std::string icon = "");

	private:
		// Override mw::Window
		void update(Uint32 deltaTime) override;

		// Override mw::Window
		void eventUpdate(const SDL_Event& windowEvent) override;

		void resize();

		void init();

		std::queue<SDL_Event> eventQueue_;
	};

} // Namespace gui.

#endif // FRAME_H
