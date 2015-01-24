#include "boardshaderfunctions.h"

#include <algorithm>

namespace {

	inline void addVertex(GLfloat* data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		const mw::Color& color) {

		data[index++] = x;
		data[index++] = y;

		data[index++] = xTex;
		data[index++] = yTex;

		data[index++] = isTex;

		data[index++] = color.red_;
		data[index++] = color.green_;
		data[index++] = color.blue_;
		data[index++] = color.alpha_;
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(GLfloat* data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		const mw::Color& color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

} // Namespace mw.

// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
void addSquareToBoardShader(GLfloat* data, int& index,
	float x, float y,
	float w, float h,
	const mw::Color& color) {

	// Left triangle |_
	addTriangle(data, index,
		x, y,
		x + w, y,
		x, y + h,
		0, 0,
		0, 0,
		0, 0,
		false,
		color);
	//                _
	// Right triangle  |
	addTriangle(data, index,
		x, y + h,
		x + w, y,
		x + w, y + h,
		0, 0,
		0, 0,
		0, 0,
		false,
		color);
}

void addSquareToBoardShader(GLfloat* data, int& index,
	float x, float y,
	float w, float h,
	const mw::Sprite& sprite, const mw::Color& color) {
	int textureW = sprite.getTexture().getWidth();
	int textureH = sprite.getTexture().getHeight();

	// Left triangle |_
	addTriangle(data, index,
		x, y,
		x + w, y,
		x, y + h,
		sprite.getX() / textureW, sprite.getY() / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
		sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		true,
		color);
	//                _
	// Right triangle  |
	addTriangle(data, index,
		x, y + h,
		x + w, y,
		x + w, y + h,
		sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		true,
		color);
}

void addSquareToBoardShader(GLfloat* data, int& index,
	float dx, float dy) {
	
	for (int i = 0; i < 4; ++i) {
		data[index] += dx;
		data[index + 1] += dy;
		index += 9;
	}
}

void setVertexAttribPointer(const BoardShader& shader) {
	shader.setGlPosA(2, sizeof(GLfloat) * 9, (GLvoid*) 0);
	shader.setGlTexA(2, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
	shader.setGlIsTexA(1, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));
	shader.setGlColorA(4, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
}
