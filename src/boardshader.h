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

	void setGlPosA(GLint size, const GLvoid* data) const;
	void setGlPosA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlTexA(GLint size, const GLvoid* data) const;
	void setGlTexA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlIsTexA(GLint size, const GLvoid* data) const;
	void setGlIsTexA(GLint size, GLsizei stride, const GLvoid* data) const;

	void setGlColorA(GLint size, const GLvoid* data) const;
	void setGlColorA(GLint size, GLsizei stride, const GLvoid* data) const;

	// Uniforms. -------------------------------------------

	void setGlMatrixU(const mw::Matrix44& matrix) const;	

private:
	mw::Shader shader_;

	int aPosIndex_;
	int aTexIndex_;
	int aIsTexIndex_;
	int aColorIndex_;

	int uMatrixIndex_;
};

#endif // BOARDSHADER_H
