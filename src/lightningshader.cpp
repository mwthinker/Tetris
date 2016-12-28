#include "lightningshader.h"

LightningShader::LightningShader() {
	aAngle_ = -1;
	aPos_ = -1;
	aTex_ = -1;
	aLocalPos_ = -1;
	uColor_ = -1;
	uMat_ = -1;
}
	
LightningShader::LightningShader(std::string vShaderFile, std::string fShaderFile) {
	shader_.bindAttribute("aAngle");
	shader_.bindAttribute("aPos");
	shader_.bindAttribute("aTex");
	shader_.bindAttribute("aLocalPos");
	shader_.loadAndLinkFromFile(vShaderFile, fShaderFile);
	
	shader_.useProgram();

	aAngle_ = shader_.getAttributeLocation("aAngle");
	aPos_ = shader_.getAttributeLocation("aPos");
	aTex_ = shader_.getAttributeLocation("aTex");
	aLocalPos_ = shader_.getAttributeLocation("aLocalPos");

	// Collect the vertex buffer uniforms indexes.
	uColor_ = shader_.getUniformLocation("uColor");
	uMat_ = shader_.getUniformLocation("uMat");
}

void LightningShader::useProgram() const {
	shader_.useProgram();
}

void LightningShader::setVertexAttribPointer() const {
	glEnableVertexAttribArray(aAngle_);
	glVertexAttribPointer(aAngle_, 1, GL_FLOAT, GL_FALSE, vertexSizeInBytes(), (GLvoid*) (sizeof(GLfloat) * 0));
	glEnableVertexAttribArray(aPos_);
	glVertexAttribPointer(aPos_, 2, GL_FLOAT, GL_FALSE, vertexSizeInBytes(), (GLvoid*) (sizeof(GLfloat) * 1));
	glEnableVertexAttribArray(aTex_);
	glVertexAttribPointer(aTex_, 2, GL_FLOAT, GL_FALSE, vertexSizeInBytes(), (GLvoid*) (sizeof(GLfloat) * 3));
	glEnableVertexAttribArray(aLocalPos_);
	glVertexAttribPointer(aLocalPos_, 2, GL_FLOAT, GL_FALSE, vertexSizeInBytes(), (GLvoid*) (sizeof(GLfloat) * 5));
	mw::checkGlError();
}

void LightningShader::setUMat(const Mat44& matrix) const {
	shader_.useProgram();
	glUniformMatrix4fv(uMat_, 1, false, matrix.data());
}

void LightningShader::setUColor(const Color& color) const {
	shader_.useProgram();
	glUniform4f(uColor_, color.red_, color.green_, color.blue_, color.alpha_);
}
