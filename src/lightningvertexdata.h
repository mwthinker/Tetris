#ifndef LIGHTNINGVERTEXDATA_H
#define LIGHTNINGVERTEXDATA_H

#include "lightningshader.h"
#include "vertexdata.h"

#include "mat44.h"
#include "color.h"

#include <mw/sprite.h>

class LightningVertexData : public VertexData {
public:
	LightningVertexData(const LightningShader& lightningShader) : lightningShader_(lightningShader) {
	}

	void setColor(const Color& color) {
		lightningShader_.setUColor(color);
	}

	void setMatrix(const Mat44& mat) {
		lightningShader_.setUMat(mat);
	}

	void addSegmentTRIANGLES(Vec2 start, Vec2 end, float thickness, const mw::Sprite& startSprite, const mw::Sprite& middleSprite, const mw::Sprite& endSprite) {
		Vec2 tangent = end - start;
		float angle = tangent.angle();
		float segmentLength = tangent.magnitude();
		Vec2 leftMiddle = -thickness * 0.5f * tangent / segmentLength + start;
		Vec2 middle = start + tangent * 0.5f;
		Vec2 rightMiddle = thickness * 0.5f * tangent / segmentLength + end;
		
		addSquareTRIANGLES(angle, leftMiddle.x_, leftMiddle.y_, -0.5f * thickness, -0.5f * thickness, thickness, thickness, startSprite);
		addSquareTRIANGLES(angle, middle.x_, middle.y_, -0.5f * segmentLength, -0.5f * thickness, segmentLength, thickness, middleSprite);
		addSquareTRIANGLES(angle+3.14f, rightMiddle.x_, rightMiddle.y_, -0.5f * thickness, -0.5f * thickness, thickness, thickness, endSprite);
	}

	void addVertex(float angle, float x, float y, float localX, float localY, float xTex, float yTex) {
		VertexData::addVertex<7>({angle, x, y, xTex, yTex, localX, localY});
	}

	void addSquareTRIANGLES(float angle, float x,  float y, float localX, float localY, float w, float h, const mw::Sprite& sprite) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		// Left triangle |_
		addTriangleTRIANGLES(angle,
			x, y,
			localX, localY,
			localX + w, localY,
			localX, localY + h,
			sprite.getX() / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH);
		//
		// Right triangle  |
		addTriangleTRIANGLES(angle,
			x, y,
			localX, localY + h,
			localX + w, localY,
			localX + w, localY + h,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH);
	}

	void addTriangleTRIANGLES(
		float angle,
		float x, float y,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3) {

		addVertex(angle, x, y, x1, y1, xTex1, yTex1);
		addVertex(angle, x, y, x2, y2, xTex2, yTex2);
		addVertex(angle, x, y, x3, y3, xTex3, yTex3);
	}

	void setVertexAttribPointer() const override {
		lightningShader_.setVertexAttribPointer();
	}

	unsigned int vertexSizeInFloat() const override {
		return lightningShader_.vertexSizeInFloat();
	}

private:
	LightningShader lightningShader_;
};

#endif // LIGHTNINGVERTEXDATA_H
