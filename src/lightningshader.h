#ifndef LIGHTNINGSHADER_H
#define LIGHTNINGSHADER_H

#include "mat44.h"

#include <mw/shader.h>
#include <mw/color.h>

#include <string>

class LightningShader {
public:
	static constexpr unsigned int vertexSizeInBytes() {
		return vertexSizeInFloat() * sizeof(GLfloat);
	}

	static constexpr unsigned int vertexSizeInFloat() {
		return 7;
	}

	LightningShader();

	LightningShader(std::string vShaderFile, std::string fShaderFile);

	void useProgram() const;

	void setVertexAttribPointer() const;

	void setUMat(const Mat44& matrix) const;
	void setUColor(const mw::Color& color) const;
	

private:
	int aAngle_;
	int aPos_;
	int aTex_;
	int aLocalPos_;
	int uMat_;
	int uColor_;
	int uTexture_;
	mw::Shader shader_;
};

#endif // LIGHTNINGSHADER_H
