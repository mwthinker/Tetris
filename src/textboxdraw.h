#ifndef TEXTBOXDRAW_H
#define TEXTBOXDRAW_H

#include "textbox.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>
#include <memory>

namespace gui {

	class TextBoxDraw : public TextBox {
	public:
		TextBoxDraw(int width, mw::FontPtr font, int characterSize,  mw::Color textColor, mw::Color focused, mw::Color notFoxused);

		void draw() override;

	private:	
		mw::Text text_;
		mw::Text marker_;
		mw::Color textColor_, focused_, notFoxused_;
	};

	typedef std::shared_ptr<TextBoxDraw> TextBoxDrawPtr;

	TextBoxDrawPtr createTextBoxDraw(int width, mw::FontPtr font, int characterSize,  mw::Color textColor, mw::Color focused, mw::Color notFoxused);

} // Namespace gui.

#endif // TEXTBOXDRAW_H