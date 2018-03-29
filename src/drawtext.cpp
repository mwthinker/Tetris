#include "drawtext.h"

DrawText::DrawText(std::string text, const mw::Font& font, float lowX, float lowY, float scale, bool center)
	: lowX_(lowX), lowY_(lowY), center_(center) {

	text_ = mw::Text(text, font, scale);
	updateVertexes();
}

DrawText::DrawText(float lowX, float lowY)
	: lowX_(lowX), lowY_(lowY), center_(true) {
}

void DrawText::update(std::string text) {
	if (text_.getText() != text) {
		update(text, lowX_, lowY_);
	}
}

void DrawText::update(const mw::Text& text) {
	if (text_ != text) {
		text_ = text;
		vertexes_.clear();
		updateVertexes();
	}
}

void DrawText::update(std::string text, float x, float y) {
	lowX_ = x;
	lowY_ = y;
	text_.setText(text);
	vertexes_.clear();
	updateVertexes();
}

void DrawText::update(float x, float y) {
	lowX_ = x;
	lowY_ = x;
	update(text_.getText(), x, y);
}

void DrawText::bindTexture() {
	text_.bindTexture();
}

void DrawText::updateVertexes() {
	addRectangle(vertexes_,
		getLowX(), getLowY(),
		text_.getWidth(),
		text_.getHeight(),
		mw::Sprite(text_.getTexture())
	);
}
