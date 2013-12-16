#ifndef FRAME_H
#define FRAME_H

#include "component.h"
#include "button.h"
#include "panel.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/color.h>

#include <string>
#include <array>
#include <functional>
#include <queue>

namespace gui {

	class TextField {
	public:
		TextField(std::string strInitialText);

		// Get the current text on this TextField instance.
		std::string getText() const;
		
		// Set the display text on this TextField instance.
		void setText(std::string strText);
		
		// Set this TextField to editable (read/write) or non-editable (read-only).
		void setEditable(bool editable);
	};

	class Label : public Component {
	public:
		enum Alignment {
			LEFT,
			CENTER,
			RIGHT
		};

		Label(std::string strLabel, Alignment alignment) {
		}

		std::string getText();
		void setText(std::string strLabel);
		
		Alignment getAlignment();
		void setAlignment(Alignment alignment);
	};

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
