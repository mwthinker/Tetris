#include "textitem.h"
#include "color.h"

#include <mw/text.h>
#include <mw/font.h>

namespace gui {

	TextItem::TextItem(std::string text, mw::FontPtr font, int size, Color color) : text_(text, font, size), color_(color) {
		setWidth((int)text_.getWidth());
		setHeight((int)text_.getHeight());
	}

	void TextItem::draw() {
		color_.glColor4d();
		text_.draw();
	}

} // Namespace gui.
