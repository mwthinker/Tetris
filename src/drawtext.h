#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include "boardvertexdata.h"
#include "boardshader.h"

#include <mw/text.h>

class DrawText;
using DrawTextPtr = std::shared_ptr<DrawText>;

class DrawText : public BoardVertexData {
public:
	DrawText(const BoardShaderPtr& shader, std::string text, mw::Font& font, float lowX, float lowY, float scale);

	void update(std::string text);

	void update(std::string text, float x, float y);

	void update(float x, float y);

	void draw(float deltaTime);

private:
	mw::Text text_;

	float lowX_, lowY_;
};

#endif // DRAWTEXT_H
