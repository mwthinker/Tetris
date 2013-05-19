#ifndef TEXTBUTTON_H
#define TEXTBUTTON_H

#include "button.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <memory>

namespace gui {

	class TextButton : public Button {
	public:
		TextButton(std::string text, int size, mw::FontPtr font);
		TextButton(std::string text, int size, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover);

		void setText(std::string text);
		std::string getText() const;

		// Override View
		void draw() override;

		void setTextSize(int size);

	private:
		mw::Color textColor_, focused_, onHover_, notHover_, insideDownHover_;
		mw::Text text_;
		int size_;
	};
	
	typedef std::shared_ptr<TextButton> TextButtonPtr;

	TextButtonPtr createTextButton(std::string text, int size, mw::FontPtr font);
	
	TextButtonPtr createTextButton(std::string text, int size, mw::FontPtr font, mw::Color textColor, mw::Color focused, mw::Color onHover,
			mw::Color notHover, mw::Color insideDownHover);
	
} // Namespace gui.

#endif // TEXTBUTTON_H
