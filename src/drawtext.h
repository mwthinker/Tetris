#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include "dynamicbuffer.h"
#include "vertexdata.h"
#include "boardshader.h"

#include <mw/text.h>

class DrawText;
using DrawTextPtr = std::shared_ptr<DrawText>;

class DrawText {
public:
	DrawText(DynamicBuffer& buffer, std::string text, mw::Font& font, float lowX, float lowY);

	void update(std::string text, float scale);

	void update(std::string text);

	void draw(float deltaTime, const BoardShader& shader);

private:
	mw::Text text_;

	float lowX_, lowY_;
	float timeLeft_;

	VertexDataPtr vd_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};
#endif // DRAWTEXT_H
