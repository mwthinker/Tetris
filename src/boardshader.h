#ifndef BOARDSHADER_H
#define BOARDSHADER_H

#include <mw/opengl.h>
#include <mw/matrix.h>
#include <mw/shader.h>

#include <mw/color.h>
#include <mw/sprite.h>

class BoardShader {
public:
	BoardShader();
	BoardShader(std::string vShaderFile, std::string fShaderFile);

	void glUseProgram() const;

	// Vertex buffer Attributes. ---------------------------

	void setGlVerA(const GLvoid* data) const;
	void setGlVerA(GLsizei stride, const GLvoid* data) const;

	void setGlTexA(const GLvoid* data) const;
	void setGlTexA(GLsizei stride, const GLvoid* data) const;

	void setGlIsTexA(const GLvoid* data) const;
	void setGlIsTexA(GLsizei stride, const GLvoid* data) const;

	void setGlColorA(const GLvoid* data) const;
	void setGlColorA(GLsizei stride, const GLvoid* data) const;

	// Uniforms. -------------------------------------------

	void setGlMatrixU(const mw::Matrix44& matrix) const;

	static inline void addVertex(GLfloat* data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		mw::Color color) {

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
	static inline void addTriangle(GLfloat* data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		mw::Color color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	static inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Color color) {

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

	static inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Sprite& sprite, mw::Color color = mw::Color(1, 1, 1)) {
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

private:
	mw::Shader shader_;

	int aVerIndex_;
	int aTexIndex_;
	int aIsTexIndex_;
	int aColorIndex_;

	int uMatrixIndex_;
};

#endif // BOARDSHADER_H
