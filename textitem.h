#ifndef TEXTITEM_H
#define TEXTITEM_H

#include "guiitem.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

#include <SDL_opengl.h>

#include <memory>

namespace gui {

	class TextItem : public GuiItem {
	public:
		TextItem(std::string text, mw::FontPtr font, int size, mw::Color color = mw::Color(1,1,1));

		virtual void draw() override;

		inline mw::Text getText() const {
			return text_;
		}

	private:
		mw::Text text_;
		mw::Color color_;
	};

	typedef std::shared_ptr<TextItem> TextItemPtr;

	TextItemPtr createTextItem(std::string text, mw::FontPtr font, int size, mw::Color color);

} // Namespace gui.

#endif // TEXTITEM_H
