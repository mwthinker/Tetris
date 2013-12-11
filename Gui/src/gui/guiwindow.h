#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "component.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/color.h>

#include <string>
#include <array>
#include <functional>
#include <queue>

namespace gui {

	class Button {
	public:
		Button(std::string buttonLabel) {
		}

		std::string getLabel() const;
		
		// Get the label of this Button instance
		void setLabel(std::string buttonLabel);
		
		// Set the label of this Button instance.
		void setEnable(bool enable);
	};

	class TextField {
	public:
		TextField(std::string strInitialText);

		std::string getText() const;
		// Get the current text on this TextField instance
		void setText(std::string strText);
		// Set the display text on this TextField instance
		void setEditable(bool editable);
		// Set this TextField to editable (read/write) or non-editable (read-only)
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
	class Frame : public mw::Window, public Container {
	public:
		Frame();

		void add() {
		}

	private:
		// Override mw::Window
		void update(Uint32 deltaTime) override;

		// Override mw::Window
		void eventUpdate(const SDL_Event& windowEvent) override;

		void resize();

		std::queue<SDL_Event> eventQueue_;
	};

} // Namespace gui.

#endif // GUIWINDOW_H
