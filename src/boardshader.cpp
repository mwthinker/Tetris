#include "boardshader.h"

BoardShader::BoardShader() {
	aVerIndex_ = -1;
	aTexIndex_ = -1;
	aIsTexIndex_ = -1;
	aColorIndex_ = -1;

	// Collect the vertex buffer uniforms indexes.
	uMatrixIndex_ = -1;
}

BoardShader::BoardShader(std::string vShaderFile, std::string fShaderFile) {
	shader_.bindAttribute("aVer");
	shader_.bindAttribute("aTex");
	shader_.bindAttribute("aIsTex");
	shader_.bindAttribute("aColor");
	shader_.loadAndLinkFromFile(vShaderFile, fShaderFile);

	shader_.glUseProgram();

	// Collect the vertex buffer attributes indexes.
	aVerIndex_ = shader_.getAttributeLocation("aVer");
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

void BoardShader::setGlVerA(const GLvoid* data) const {
	mw::glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
	mw::glEnableVertexAttribArray(aVerIndex_);
}

void BoardShader::setGlVerA(GLsizei stride, const GLvoid* data) const {
	mw::glVertexAttribPointer(aVerIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
	mw::glEnableVertexAttribArray(aVerIndex_);
}

void BoardShader::setGlTexA(const GLvoid* data) const {
	mw::glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, 0, data);
	mw::glEnableVertexAttribArray(aTexIndex_);
}

void BoardShader::setGlTexA(GLsizei stride, const GLvoid* data) const {
	mw::glVertexAttribPointer(aTexIndex_, 2, GL_FLOAT, GL_FALSE, stride, data);
	mw::glEnableVertexAttribArray(aTexIndex_);
}

void BoardShader::setGlIsTexA(const GLvoid* data) const {
	mw::glVertexAttribPointer(aIsTexIndex_, 1, GL_FLOAT, GL_FALSE, 0, data);
	mw::glEnableVertexAttribArray(aIsTexIndex_);
}

void BoardShader::setGlIsTexA(GLsizei stride, const GLvoid* data) const {
	mw::glVertexAttribPointer(aIsTexIndex_, 1, GL_FLOAT, GL_FALSE, stride, data);
	mw::glEnableVertexAttribArray(aIsTexIndex_);
}

void BoardShader::setGlColorA(const GLvoid* data) const {
	mw::glVertexAttribPointer(aColorIndex_, 4, GL_FLOAT, GL_FALSE, 0, data);
	mw::glEnableVertexAttribArray(aColorIndex_);
}

void BoardShader::setGlColorA(GLsizei stride, const GLvoid* data) const {
	mw::glVertexAttribPointer(aColorIndex_, 4, GL_FLOAT, GL_FALSE, stride, data);
	mw::glEnableVertexAttribArray(aColorIndex_);
}

// Uniforms. -------------------------------------------

void BoardShader::setGlMatrixU(const mw::Matrix44& matrix) const {
	mw::glUniformMatrix4fv(uMatrixIndex_, 1, false, matrix.data());
}
