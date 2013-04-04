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

		void setText(std::string text);
		std::string getText() const;

		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override;

		bool isMouseInside() const;

		bool isPushed() const;

		bool isMouseDown() const;

		void click();

		void addOnClickListener(std::function<void()> onClick);

	private:
		void excecute();

		mw::Signal<void> onClick_;
		std::string text_;
		bool pushed_;
		bool mouseDown_;
		bool mouseInside_;
	};

	typedef std::shared_ptr<Button> ButtonPtr;

} // Namespace gui.

#endif // BUTTON_H
