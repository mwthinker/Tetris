#ifndef BOARDSHADER_H
#define BOARDSHADER_H

#include <mw/shader.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include "mat44.h"

#include <memory>

class BoardShader;
using BoardShaderPtr = std::shared_ptr<BoardShader>;

class BoardShader {
public:
	static constexpr unsigned int vertexSizeInBytes() {
		return sizeof(Vertex);
	}

	static constexpr unsigned int vertexSizeInFloat() {
		return sizeof(Vertex) / sizeof(GLfloat);
	}

	BoardShader();
	BoardShader(std::string vShaderFile, std::string fShaderFile);

	void useProgram() const;

	void setVertexAttribPointer() const;

	// Uniforms. -------------------------------------------
	void setMatrix(const Mat44& matrix) const;

	class Vertex {
	public:
		Vertex() = default;

		Vertex(GLfloat x, GLfloat y) : pos_(x, y), color_(1, 1, 1, 1), texture_(0) {
		}

		Vertex(GLfloat x, GLfloat y, const mw::Color& color) : pos_(x, y), color_(color), texture_(0) {
		}

		Vertex(GLfloat x, GLfloat y, GLfloat xTex, GLfloat yTex) : pos_(x, y), tex_(xTex, yTex), color_(1, 1, 1, 1), texture_(1.f) {
		}

		Vertex(GLfloat x, GLfloat y, GLfloat xTex, GLfloat yTex, const mw::Color& color) : pos_(x, y), tex_(xTex, yTex), color_(color), texture_(1.f) {
		}

		bool isTexture() const {
			return texture_;
		}

		// The order is important for setVertexAttribPointer()
		Vec2 pos_;
		Vec2 tex_;
		GLfloat texture_;
		mw::Color color_;
	};

private:
	mw::Shader shader_;

	int uMatrixIndex_;

	// Vertex buffer Attributes. ---------------------------
	int aPosIndex_;
	int aTexIndex_;
	int aTextureIndex_;
	int aColorIndex_;
};

#endif // BOARDSHADER_H
