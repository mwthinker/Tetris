#ifndef BOARDSHADERFUNCTIONS_H
#define BOARDSHADERFUNCTIONS_H

#include "boardshader.h"

#include <mw/sprite.h>
#include <mw/color.h>

void addSquareToBoardShader(GLfloat* data, int& index,
	float x, float y,
	float w, float h,
	const mw::Color& color);

void addSquareToBoardShader(GLfloat* data, int& index,
	float x, float y,
	float w, float h,
	const mw::Sprite& sprite, const mw::Color& color = mw::Color(1, 1, 1));

void setVertexAttribPointer(const BoardShader& shader);

#endif // BOARDSHADERFUNCTIONS_H
