#include "boardshader.h"

BoardShader::BoardShader() {
	aPosIndex_ = -1;
	aTexIndex_ = -1;
	aIsTexIndex_ = -1;
	aColorIndex_ = -1;

	// Collect the vertex buffer uniforms indexes.
	uMatrixIndex_ = -1;
}

BoardShader::BoardShader(std::string vShaderFile, std::string fShaderFile) {
	shader_.bindAttribute("aPos");
	shader_.bindAttribute("aTex");
	shader_.bindAttribute("aIsTex");
	shader_.bindAttribute("aColor");
	shader_.loadAndLinkFromFile(vShaderFile, fShaderFile);

	shader_.glUseProgram();

	// Collect the vertex buffer attributes indexes.
	aPosIndex_ = shader_.getAttributeLocation("aPos");
	aTexIndex_ = shader_.getAttributeLocation("aTex");
	aIsTexIndex_ = shader_.getAttributeLocation("aIsTex");
	aColorIndex_ = shader_.getAttributeLocation("aColor");

	// Collect the vertex buffer uniforms indexes.
	uMatrixIndex_ = shader_.getUniformLocation("uMat");
}

void BoardShader::glUseProgram() const {
	shader_.glUseProgram();
}

// Vertex buffer Attributes. ---------------------------

void BoardShader::setGlPosA(GLint size, const GLvoid* data) const {
	glEnableVertexAttribArray(aPosIndex_);
	glVertexAttribPointer(aPosIndex_, size, GL_FLOAT, GL_FALSE, 0, data);
}

void BoardShader::setGlPosA(GLint size, GLsizei stride, const GLvoid* data) const {
	glEnableVertexAttribArray(aPosIndex_);
	glVertexAttribPointer(aPosIndex_, size, GL_FLOAT, GL_FALSE, stride, data);
}

void BoardShader::setGlTexA(GLint size, const GLvoid* data) const {
	glEnableVertexAttribArray(aTexIndex_);
	glVertexAttribPointer(aTexIndex_, size, GL_FLOAT, GL_FALSE, 0, data);
}

void BoardShader::setGlTexA(GLint size, GLsizei stride, const GLvoid* data) const {
	glEnableVertexAttribArray(aTexIndex_);
	glVertexAttribPointer(aTexIndex_, size, GL_FLOAT, GL_FALSE, stride, data);
}

void BoardShader::setGlIsTexA(GLint size, const GLvoid* data) const {
	glEnableVertexAttribArray(aIsTexIndex_);
	glVertexAttribPointer(aIsTexIndex_, size, GL_FLOAT, GL_FALSE, 0, data);
}

void BoardShader::setGlIsTexA(GLint size, GLsizei stride, const GLvoid* data) const {
	glEnableVertexAttribArray(aIsTexIndex_);
	glVertexAttribPointer(aIsTexIndex_, size, GL_FLOAT, GL_FALSE, stride, data);
}

void BoardShader::setGlColorA(GLint size, const GLvoid* data) const {
	glEnableVertexAttribArray(aColorIndex_);
	glVertexAttribPointer(aColorIndex_, size, GL_FLOAT, GL_FALSE, 0, data);
}

void BoardShader::setGlColorA(GLint size, GLsizei stride, const GLvoid* data) const {
	glEnableVertexAttribArray(aColorIndex_);
	glVertexAttribPointer(aColorIndex_, size, GL_FLOAT, GL_FALSE, stride, data);
}

// Uniforms. -------------------------------------------

void BoardShader::setGlMatrixU(const mw::Matrix44& matrix) const {
	glUniformMatrix4fv(uMatrixIndex_, 1, false, matrix.data());
}
