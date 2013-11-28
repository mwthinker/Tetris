#ifndef BUTTON_H
#define BUTTON_H

#include "widget.h"

#include <mw/signal.h>

#include <functional>
#include <string>

namespace gui {

	class Button : public Widget {
	public:
		Button();

		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override;

		bool isMouseInside() const;

		bool isPushed() const;

		bool isMouseDown() const;

	private:
		void excecute();		
		
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};	

} // Namespace gui.

#endif // BUTTON_H
