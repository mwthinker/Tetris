#ifndef BOARDSHADER_H
#define BOARDSHADER_H

#include <mw/opengl.h>
#include <mw/matrix.h>
#include <mw/shader.h>

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

private:
	mw::Shader shader_;

	int aVerIndex_;
	int aTexIndex_;
	int aIsTexIndex_;
	int aColorIndex_;

	int uMatrixIndex_;
};

#endif // BOARDSHADER_H
