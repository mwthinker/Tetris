#include "drawtext.h"

DrawText::DrawText(std::string text, mw::Font& font, float lowX, float lowY, float scale) : lowX_(lowX), lowY_(lowY) {
	text_ = mw::Text(text, font, scale);

	update(text);
}

void DrawText::update(std::string text) {
	text_.setText(text);
	vertexes_.clear();
	addRectangle(vertexes_,
		lowX_, lowY_,
		text_.getWidth(),
		text_.getHeight(),
		mw::Sprite(text_.getTexture()));
}

void DrawText::update(std::string text, float x, float y) {
	lowX_ = x;
	lowY_ = x;
	update(text);
}

void DrawText::update(float x, float y) {
	lowX_ = x;
	lowY_ = x;
	update(text_.getText(), x, y);
}

void DrawText::bindTexture() {
	text_.bindTexture();
}
