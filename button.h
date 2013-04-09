#ifndef BUTTON_H
#define BUTTON_H

#include "guiitem.h"

#include <mw/signal.h>

#include <functional>
#include <string>

namespace gui {

	class Button : public GuiItem {
	public:
		Button();
		Button(std::string text);		

		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override;

		bool isMouseInside() const;

		bool isPushed() const;

		bool isMouseDown() const;

		void click();
	private:
		void excecute();		
		
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};

	typedef std::shared_ptr<Button> ButtonPtr;

} // Namespace gui.

#endif // BUTTON_H
