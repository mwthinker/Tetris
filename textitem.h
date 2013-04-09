#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "guiitem.h"
#include "color.h"

#include <mw/text.h>
#include <mw/font.h>

#include <SDL_opengl.h>

namespace gui {

	class TextItem : public GuiItem {
	public:
		TextItem(std::string text, mw::FontPtr font, int size, Color color = Color(1,1,1));

		virtual void draw() override;

	private:
		Color color_;
		mw::FontPtr font_;
		mw::Text text_;
	};	

} // Namespace gui.

#endif // TEXTITEM_H
