#ifndef TEXTBOX_H
#define TEXTBOX_H

#include "widget.h"
#include "inputformatter.h"

#include <mw/signal.h>

#include <string>

namespace gui {

	class TextBox : public Widget {
	public:
		TextBox(int width, int height);

		void setInputFormatter(const InputFormatterPtr& inputFormatter);

		void setText(std::string str);

		// Override GuiItem.
		void eventUpdate(const SDL_Event& windowEvent, int x, int y) override;

		bool isMouseInside() const;

		std::string getText() const;

		int getMarkerPosition() const;

	private:
		mw::Signal<std::string> excecute_;
		InputFormatterPtr inputFormatter_;
		bool inside_;
	};

} // Namespace gui.

#endif // TEXTBOX_H
