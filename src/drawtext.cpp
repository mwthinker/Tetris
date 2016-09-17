#include "drawtext.h"

DrawText::DrawText(DynamicBuffer& buffer, std::string text, mw::Font& font, float lowX, float lowY) : lowX_(lowX), lowY_(lowY) {
	vd_ = buffer.pollFirstFree();

	text_ = mw::Text(text, font);

	update(text, (float) font.getCharacterSize());
}

void DrawText::update(std::string text, float scale) {
	text_.setText(text);
	text_.setCharacterSize(scale);
	vd_->begin();
	vd_->addSquareTRIANGLES(lowX_, lowY_, text_.getWidth(), text_.getHeight(), mw::Sprite(text_.getTexture()));
	vd_->end();
}

void DrawText::update(std::string text) {
	update(text, text_.getCharacterSize());
}

void DrawText::draw(float deltaTime, const BoardShader& shader) {
	text_.bindTexture();
	vd_->drawTRIANGLES(shader);
}

