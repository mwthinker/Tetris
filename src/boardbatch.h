#ifndef BOARDBATCH_H
#define BOARDBATCH_H

#include "boardshader.h"

#include <mw/batch.h>
#include <mw/sprite.h>
#include <mw/window.h>

class BoardBatch : public mw::Batch<BoardShader> {
public:
	BoardBatch(const std::shared_ptr<BoardShader>& shader, int maxVertexes) : Batch(GL_TRIANGLES, GL_DYNAMIC_DRAW, shader, maxVertexes) {
		std::cout << getVboSizeInMiB() << " Mib\n";
	}

	BoardBatch(const std::shared_ptr<BoardShader>& shader) : Batch(GL_TRIANGLES, shader) {
	}

	virtual ~BoardBatch() = default;

	void addTriangle(const BoardShader::Vertex& v1, const BoardShader::Vertex& v2, const BoardShader::Vertex& v3) {
		Batch::add(v1);
		Batch::add(v2);
		Batch::add(v3);
	}

	void addRectangle(float x, float y, float w, float h, const mw::Color& color) {
		addTriangle(BoardShader::Vertex(x, y, color), BoardShader::Vertex(x + w, y, color), BoardShader::Vertex(x, y + h, color));
		addTriangle(BoardShader::Vertex(x, y + h, color), BoardShader::Vertex(x + w, y, color), BoardShader::Vertex(x + w, y + h, color));
	}

	void addSquare(float x, float y, float size, const mw::Color& color) {
		addRectangle(x, y, size, size, color);
	}

	void addRectangle(float x, float y, float w, float h, const mw::Sprite& sprite) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		addTriangle(BoardShader::Vertex(x, y, sprite.getX() / textureW, sprite.getY() / textureH),
			BoardShader::Vertex(x + w, y, (sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH),
			BoardShader::Vertex(x, y + h, sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH));

		addTriangle(BoardShader::Vertex(x, y + h, sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH),
			BoardShader::Vertex(x + w, y, (sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH),
			BoardShader::Vertex(x + w, y + h, (sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH));
	}

	void addSquare(float x, float y, float size, const mw::Sprite& sprite, const mw::Color& color) {
		addRectangle(x, y, size, size, sprite, color);
	}

	void addRectangle(float x, float y, float w, float h, const mw::Sprite& sprite, const mw::Color& color) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		addTriangle(BoardShader::Vertex(x, y, sprite.getX() / textureW, sprite.getY() / textureH, color),
			BoardShader::Vertex(x + w, y, (sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH, color),
			BoardShader::Vertex(x, y + h, sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH, color));

		addTriangle(BoardShader::Vertex(x, y + h, sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH, color),
			BoardShader::Vertex(x + w, y, (sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH, color),
			BoardShader::Vertex(x + w, y + h, (sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH, color));
	}

	void addRectangle(const BoardShader::Vertex& v1, const BoardShader::Vertex& v2, const BoardShader::Vertex& v3, const BoardShader::Vertex& v4) {
		addTriangle(v1, v2, v3);
		addTriangle(v3, v4, v1);
	}

	void addCircle(float x, float y, float radius, const int iterations = 40) {
		const float PI = 3.14159265359f;

		const BoardShader::Vertex v1(x, y);
		BoardShader::Vertex v2(x + radius, y);
		for (int i = 1; i <= iterations; ++i) {
			BoardShader::Vertex v3(x + radius * std::cos(2 * PI * i / iterations), y + radius * std::sin(2 * PI * i / iterations));
			addTriangle(v1, v2, v3);
			v2 = v3;
		}
	}

	void addLine(float x1, float y1, float x2, float y2, float w) {
		float angle = atan2(x2 - x1, y2 - y1);
		float dx = 0.5f * w * std::cos(angle);
		float dy = 0.5f * w * std::sin(angle);
		addRectangle(BoardShader::Vertex(x1 - dx, y1 + dy), BoardShader::Vertex(x1 + dx, y1 - dy), BoardShader::Vertex(x2 + dx, y2 - dy), BoardShader::Vertex(x2 - dx, y2 + dy));
	}	

};

#endif // BOARDBATCH_H
