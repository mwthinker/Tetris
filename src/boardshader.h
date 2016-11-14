#ifndef BOARDSHADER_H
#define BOARDSHADER_H

#include <mw/shader.h>
#include <mw/color.h>
#include <mw/sprite.h>

#include "mat44.h"

class BoardShader {
public:
	inline static constexpr unsigned int vertexSizeInBytes() {
		return vertexSizeInFloat() * sizeof(GLfloat);
	}

	inline static constexpr unsigned int vertexSizeInFloat() {
		return 9;
	}

	BoardShader();
	BoardShader(std::string vShaderFile, std::string fShaderFile);

	void useProgram() const;

	void setVertexAttribPointer() const;

	// Uniforms. -------------------------------------------

	void setGlMatrixU(const Mat44& matrix) const;

private:
	// Vertex buffer Attributes. ---------------------------

	void setGlPosA(GLint size, const GLvoid* data) const;
	void setGlPosA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlTexA(GLint size, const GLvoid* data) const;
	void setGlTexA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlIsTexA(GLint size, const GLvoid* data) const;
	void setGlIsTexA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlColorA(GLint size, const GLvoid* data) const;
	void setGlColorA(GLint size, GLsizei stride, const GLvoid* data) const;

	mw::Shader shader_;

	int aPosIndex_;
	int aTexIndex_;
	int aIsTexIndex_;
	int aColorIndex_;

	int uMatrixIndex_;
};

#endif // BOARDSHADER_H
