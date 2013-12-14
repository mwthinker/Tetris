#ifndef BUTTON_H
#define BUTTON_H

#include "component.h"

#include <mw/signal.h>
#include <mw/text.h>

#include <functional>
#include <string>

namespace gui {

	class Button : public Component {
	public:
		Button();
		Button(std::string text, const mw::FontPtr& font);
		Button(mw::Text text);

		// Get the label of this Button instance.
		std::string getLabel() const;

		// Set the label of this Button instance.
		void setLabel(std::string buttonLabel);

		void setFont(const mw::FontPtr& font);

		bool isMouseInside() const;

		bool isPushed() const;

		bool isMouseDown() const;

	protected:
		mw::Text text_;

	private:
		inline void init() {
			setPreferredSize(50, 50);
			std::bind(&Button::handleMouse, this, this, std::placeholders::_1);
			std::bind(&Button::handleKeyboard, this, this, std::placeholders::_1);
		}

		void handleMouse(const SDL_Event&);
		void handleKeyboard(const SDL_Event&);
		
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};

} // Namespace gui.

#endif // BUTTON_H
