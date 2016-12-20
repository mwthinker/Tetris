#include "boardvertexdata.h"

void BoardVertexData::addSquareTRIANGLES(
	float x, float y,
	float w, float h,
	const Color& color) {

	// Left triangle |_
	addTriangleTRIANGLES(
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
	addTriangleTRIANGLES(
		x, y + h,
		x + w, y,
		x + w, y + h,
		0, 0,
		0, 0,
		0, 0,
		false,
		color);
}

void BoardVertexData::addSquareTRIANGLES(float x, float y, float w, float h, const mw::Sprite& sprite, const Color& color) {
	int textureW = sprite.getTexture().getWidth();
	int textureH = sprite.getTexture().getHeight();

	// Left triangle |_
	addTriangleTRIANGLES(
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
	addTriangleTRIANGLES(
		x, y + h,
		x + w, y,
		x + w, y + h,
		sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		true,
		color);
}

void BoardVertexData::addSquareTRIANGLES(float x1, float y1,
	float x2, float y2,
	float x3, float y3,
	float x4, float y4,
	const mw::Sprite& sprite, const Color& color) {
	int textureW = sprite.getTexture().getWidth();
	int textureH = sprite.getTexture().getHeight();

	// Left triangle |_
	addTriangleTRIANGLES(
		x1, y1,
		x2, y2,
		x4, y4,
		sprite.getX() / textureW, sprite.getY() / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
		sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		true,
		color);
	//                _
	// Right triangle  |
	addTriangleTRIANGLES(
		x4, y4,
		x2, y2,
		x3, y3,
		sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
		(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
		true,
		color);
}

void BoardVertexData::addVertex(float x, float y, float xTex, float yTex, bool isTex, const Color& color) {
	VertexData::addVertex<9>({x, y, xTex, yTex, (float) isTex, color.red_, color.green_, color.blue_, color.alpha_});
}

void BoardVertexData::updateVertex(float x, float y) {
	VertexData::updateVertex(0, x);
	VertexData::updateVertex(1, y);
	nextVertex();
}

void BoardVertexData::addTriangleTRIANGLES(
	float x1, float y1,
	float x2, float y2,
	float x3, float y3,
	float xTex1, float yTex1,
	float xTex2, float yTex2,
	float xTex3, float yTex3,
	bool isTex,
	const Color& color) {

	addVertex(x1, y1, xTex1, yTex1, isTex, color);
	addVertex(x2, y2, xTex2, yTex2, isTex, color);
	addVertex(x3, y3, xTex3, yTex3, isTex, color);
}

void BoardVertexData::updateTriangleTRIANGLES(
	float x1, float y1,
	float x2, float y2,
	float x3, float y3) {

	updateVertex(x1, y1);
	updateVertex(x2, y2);
	updateVertex(x3, y3);
}

void BoardVertexData::updateSquareTRIANGLES(float x, float y, float w, float h) {
	// Left triangle |_
	updateTriangleTRIANGLES(
		x, y,
		x + w, y,
		x, y + h);
	//                _
	// Right triangle  |
	updateTriangleTRIANGLES(
		x, y + h,
		x + w, y,
		x + w, y + h);
}
