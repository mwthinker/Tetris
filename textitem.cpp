#include "textitem.h"

#include <mw/text.h>
#include <mw/font.h>
#include <mw/color.h>

namespace gui {

	TextItem::TextItem(std::string text, mw::FontPtr font, int size, mw::Color color) : text_(text, font, size), color_(color) {
		setWidth((int)text_.getWidth());
		setHeight((int)text_.getHeight());
	}

	void TextItem::draw() {
		color_.glColor4d();
		text_.draw();
	}

	TextItemPtr createTextItem(std::string text, mw::FontPtr font, int size, mw::Color color) {
		return TextItemPtr(new TextItem(text, font, size, color));
	}

} // Namespace gui.
