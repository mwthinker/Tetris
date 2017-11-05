#include "boardshader.h"

#include <mw/window.h>

BoardShader::BoardShader() : aPosIndex_(-1), aTexIndex_(-1), aTextureIndex_(-1), aColorIndex_(-1), uMatrixIndex_(-1) {
}

BoardShader::BoardShader(std::string vShaderFile, std::string fShaderFile) {
	shader_.bindAttribute("aPos");
	shader_.bindAttribute("aTex");
	shader_.bindAttribute("aColor");
	shader_.bindAttribute("aIsTex");
	shader_.loadAndLinkFromFile(vShaderFile, fShaderFile);

	shader_.useProgram();

	// Collect the vertex buffer attributes indexes.
	aPosIndex_ = shader_.getAttributeLocation("aPos");
	aTexIndex_ = shader_.getAttributeLocation("aTex");
	aColorIndex_ = shader_.getAttributeLocation("aColor");
	aTextureIndex_ = shader_.getAttributeLocation("aIsTex");

	// Collect the vertex buffer uniforms indexes.
	uMatrixIndex_ = shader_.getUniformLocation("uMat");
}

void BoardShader::setVertexAttribPointer() const {
	if (mw::Window::getOpenGlMajorVersion() >= 2) {
		int size = 0;
		glEnableVertexAttribArray(aPosIndex_);		
		glVertexAttribPointer(aPosIndex_, sizeof(Vertex::pos_)/sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(size));
		size += sizeof(Vertex::pos_);

		glEnableVertexAttribArray(aTexIndex_);
		glVertexAttribPointer(aTexIndex_, sizeof(Vertex::tex_) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(size));
		size += sizeof(Vertex::tex_);

		glEnableVertexAttribArray(aTextureIndex_);
		glVertexAttribPointer(aTextureIndex_, sizeof(Vertex::texture_) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(size));
		size += sizeof(Vertex::texture_);

		glEnableVertexAttribArray(aColorIndex_);
		glVertexAttribPointer(aColorIndex_, sizeof(Vertex::color_) / sizeof(GLfloat), GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(size));
		mw::checkGlError();
	}
}

void BoardShader::useProgram() const {
	shader_.useProgram();
}

// Uniforms. -------------------------------------------

void BoardShader::setMatrix(const Mat44& matrix) const {
	shader_.useProgram();
	glUniformMatrix4fv(uMatrixIndex_, 1, false, matrix.data());
}
