#include "vertexdata.h"
#include "boardshader.h"

#include <mw/vertexbufferobject.h>

VertexData::VertexData(int maxVertexes, int offsetInBytes, bool dynamic, const mw::VertexBufferObject& vbo) : maxVertexes_(maxVertexes), offsetInBytes_(offsetInBytes), dynamic_(dynamic), vbo_(vbo), index_(0) {
}

void VertexData::drawTRIANGLES(const BoardShader& shader) {
	int vertexes = index_ / BoardShader::vertexSizeInFloat();
	if (maxVertexes_ <= vertexes) {
		vertexes = maxVertexes_;
	}

	if (vbo_.getSize() > 0) {
		vbo_.bindBuffer();
		shader.setVertexAttribPointer();
		glDrawArrays(GL_TRIANGLES, offsetInBytes_ / BoardShader::vertexSizeInBytes(), vertexes);
		vbo_.unbindBuffer();

		mw::checkGlError();
	}
}

void VertexData::addSquareTRIANGLES(
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

void VertexData::addSquareTRIANGLES(float x, float y, float w, float h, const mw::Sprite& sprite, const Color& color) {
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

void VertexData::addSquareTRIANGLES(float x1, float y1,
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

void VertexData::addVertex(float x, float y, float xTex, float yTex, bool isTex, const Color& color) {
	if (dynamic_) {
		if (index_ + BoardShader::vertexSizeInFloat() <= maxVertexes_) {
			if (index_ < data_.size()) {
				data_[index_++] = x;
				data_[index_++] = y;

				data_[index_++] = xTex;
				data_[index_++] = yTex;

				data_[index_++] = isTex;

				data_[index_++] = color.red_;
				data_[index_++] = color.green_;
				data_[index_++] = color.blue_;
				data_[index_++] = color.alpha_;
			} else {
				data_.push_back(x);
				data_.push_back(y);

				data_.push_back(xTex);
				data_.push_back(yTex);

				data_.push_back(isTex);

				data_.push_back(color.red_);
				data_.push_back(color.green_);
				data_.push_back(color.blue_);
				data_.push_back(color.alpha_);
				index_ += 9;
			}
		}
	} else {
		data_.push_back(x);
		data_.push_back(y);

		data_.push_back(xTex);
		data_.push_back(yTex);

		data_.push_back(isTex);
		
		data_.push_back(color.red_);
		data_.push_back(color.green_);
		data_.push_back(color.blue_);
		data_.push_back(color.alpha_);
		index_ += 9;
	}
}

void VertexData::updateVertex(float x, float y) {
	if (dynamic_) {
		if (data_.size() + BoardShader::vertexSizeInFloat() <= maxVertexes_) {
			if (index_ < data_.size()) {
				data_[index_++] = x;
				data_[index_++] = y;
				index_ += BoardShader::vertexSizeInFloat() - 2;
			}
		}
	}
}

void VertexData::addTriangleTRIANGLES(
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

void VertexData::updateTriangleTRIANGLES(
	float x1, float y1,
	float x2, float y2,
	float x3, float y3) {

	updateVertex(x1, y1);
	updateVertex(x2, y2);
	updateVertex(x3, y3);
}

void VertexData::begin() {
	index_ = 0;
}

void VertexData::end() {
	if (dynamic_) {
		int size = BoardShader::vertexSizeInBytes() * maxVertexes_;
		if (vbo_.getSize() > 0 && data_.size() * sizeof(GLfloat) <= size) {
			vbo_.bindBuffer();
			vbo_.bindBufferSubData(offsetInBytes_, data_.size() * sizeof(GLfloat), data_.data());
			vbo_.unbindBuffer();
		}
	} else {
		index_ = data_.size();
		maxVertexes_ = data_.size() / BoardShader::vertexSizeInFloat();
	}
}

void VertexData::updateSquareTRIANGLES(float x, float y, float w, float h) {
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
