#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include "boardvertexdata.h"
#include "boardshader.h"

#include <mw/text.h>

class DrawText;
using DrawTextPtr = std::shared_ptr<DrawText>;

class DrawText : public BoardVertexData {
public:
	DrawText(const BoardShader& shader, std::string text, mw::Font& font, float lowX, float lowY, float scale);

	void update(std::string text);

	void draw(float deltaTime);

private:
	mw::Text text_;

	float lowX_, lowY_;
	float timeLeft_;

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};
#endif // DRAWTEXT_H
