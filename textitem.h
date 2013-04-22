#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "guiitem.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <SDL_opengl.h>

namespace gui {

	class TextItem : public GuiItem {
	public:
		TextItem(std::string text, mw::FontPtr font, int size, mw::Color color = mw::Color(1,1,1));

		virtual void draw() override;

	private:
		mw::Color color_;
		mw::FontPtr font_;
		mw::Text text_;
	};	

} // Namespace gui.

#endif // TEXTITEM_H
