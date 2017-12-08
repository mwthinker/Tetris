#include "drawtext.h"

DrawText::DrawText(std::string text, const mw::Font& font, float lowX, float lowY, float scale, bool center)
	: lowX_(lowX), lowY_(lowY), center_(center) {
	
	text_ = mw::Text(text, font, scale);
	update(text);
}

void DrawText::update(std::string text) {
	text_.setText(text);
	vertexes_.clear();
	if (center_) {
		addRectangle(vertexes_,
			lowX_ - text_.getWidth() * 0.5f, lowY_ - text_.getHeight() * 0.5f,
			text_.getWidth(),
			text_.getHeight(),
			mw::Sprite(text_.getTexture()));
	} else {
		addRectangle(vertexes_,
			lowX_, lowY_,
			text_.getWidth(),
			text_.getHeight(),
			mw::Sprite(text_.getTexture()));
	}
}

void DrawText::update(const mw::Text& text) {
	if (!(text_ == text)) {
		text_ = text;
		vertexes_.clear();
		if (center_) {
			addRectangle(vertexes_,
				lowX_ - text_.getWidth() * 0.5f, lowY_ - text_.getHeight() * 0.5f,
				text_.getWidth(),
				text_.getHeight(),
				mw::Sprite(text_.getTexture()));
		} else {
			addRectangle(vertexes_,
				lowX_, lowY_,
				text_.getWidth(),
				text_.getHeight(),
				mw::Sprite(text_.getTexture()));
		}
	}
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
