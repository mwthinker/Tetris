#ifndef TEXTBOXDRAW_H
#define TEXTBOXDRAW_H

#include "textbox.h"
#include "color.h"

#include <mw/font.h>
#include <mw/text.h>

#include <memory>
#include <string>

namespace gui {

	class TextBoxDraw : public TextBox {
	public:
		TextBoxDraw(int width, mw::FontPtr font, int characterSize,  Color textColor, Color focused, Color notFoxused);

		void draw() override;

	private:	
		mw::Text text_;
		mw::Text marker_;
		Color textColor_, focused_, notFoxused_;
	};

	typedef std::shared_ptr<TextBoxDraw> TextBoxDrawPtr;

} // Namespace gui.

#endif // TEXTBOXDRAW_H