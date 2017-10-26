#include "drawtext.h"

DrawText::DrawText(const BoardShaderPtr& shader, std::string text, mw::Font& font, float lowX, float lowY, float scale) : BoardVertexData(shader), lowX_(lowX), lowY_(lowY) {
	text_ = mw::Text(text, font, scale);

	update(text);
}

void DrawText::update(std::string text) {
	text_.setText(text);
	begin();
	addSquareTRIANGLES(lowX_, lowY_, text_.getWidth(), text_.getHeight(), mw::Sprite(text_.getTexture()));
	end();
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

void DrawText::draw(float deltaTime) {
	text_.bindTexture();
	BoardVertexData::drawTRIANGLES();
}
