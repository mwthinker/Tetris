#ifndef DRAWTEXT_H
#define DRAWTEXT_H

#include "boardshader.h"
#include "boardbatch.h"

#include <mw/text.h>

class DrawText;
using DrawTextPtr = std::shared_ptr<DrawText>;

class DrawText {
public:
	DrawText(std::string text, mw::Font& font, float lowX, float lowY, float scale);

	void update(std::string text);

	void update(std::string text, float x, float y);

	void update(float x, float y);

	void bindTexture();

	const std::vector<BoardShader::Vertex>& getVertexes() {
		return vertexes_;
	}

private:
	mw::Text text_;

	float lowX_, lowY_;
	std::vector<BoardShader::Vertex> vertexes_;
};

#endif // DRAWTEXT_H
