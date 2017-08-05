#ifndef BOARDVERTEXDATA_H
#define BOARDVERTEXDATA_H

#include "boardshader.h"

#include <mw/vertexdata.h>
#include <mw/color.h>

#include <vector>

class BoardVertexData;
using BoardVertexDataPtr = std::shared_ptr<BoardVertexData>;

class BoardVertexData : public mw::VertexData {
public:
	BoardVertexData(const BoardShader& boardShader) : boardShader_(boardShader) {
	}

	virtual ~BoardVertexData() = default;

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	void addSquareTRIANGLES(
		float x, float y,
		float w, float h,
		const mw::Color& color);

	void addSquareTRIANGLES(float x, float y, float w, float h, const mw::Sprite& sprite, const mw::Color& color = mw::Color(1, 1, 1));

	void addSquareTRIANGLES(float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float x4, float y4,
		const mw::Sprite& sprite, const mw::Color& color = mw::Color(1, 1, 1));

	void addVertex(float x, float y, float xTex, float yTex, bool isTex, const mw::Color& color);

	void addTriangleTRIANGLES(
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		const mw::Color& color);

	void updateVertex(float x, float y);

	void updateSquareTRIANGLES(float x, float y, float w, float h);

	void updateTriangleTRIANGLES(float x1, float y1,
		float x2, float y2,
		float x3, float y3);

	void setVertexAttribPointer() const override {
		boardShader_.setVertexAttribPointer();
	}

	void useProgram() const override {
		boardShader_.useProgram();
	}

	unsigned int vertexSizeInFloat() const override {
		return boardShader_.vertexSizeInFloat();
	}

	void drawTRIANGLES() {
		drawMode(GL_TRIANGLES);
	}

private:
	BoardShader boardShader_;
};

#endif // BOARDVERTEXDATA_H
