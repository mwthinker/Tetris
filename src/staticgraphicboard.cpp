#include "staticgraphicboard.h"
#include "tetrisboard.h"
#include "tetrisentry.h"
#include "boardshader.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/opengl.h>
#include <gui/component.h>

#include <sstream>
#include <vector>
#include <cassert>

namespace {

	inline void addVertex(GLfloat* data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		const mw::Color& color) {

		data[index++] = x;
		data[index++] = y;

		data[index++] = xTex;
		data[index++] = yTex;

		data[index++] = isTex;

		data[index++] = color.red_;
		data[index++] = color.green_;
		data[index++] = color.blue_;
		data[index++] = color.alpha_;
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(GLfloat* data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		const mw::Color& color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		const mw::Color& color) {

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
	}

	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Sprite& sprite, mw::Color color = mw::Color(1, 1, 1)) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			sprite.getX() / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
	}

	void setVertexAttribPointer(const BoardShader& shader) {
		shader.setGlPosA(2, sizeof(GLfloat) * 9, (GLvoid*) 0);
		shader.setGlTexA(2, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
		shader.setGlIsTexA(1, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));
		shader.setGlColorA(4, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
	}

}

StaticGraphicBoard::StaticGraphicBoard() {
}

StaticGraphicBoard::StaticGraphicBoard(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares].
	lowX_(x), lowY_(y) {

	rows_ = tetrisBoard.getRows();
	columns_ = tetrisBoard.getColumns();

	mw::Color color1 = boardEntry.getChildEntry("outerSquareColor").getColor();
	mw::Color color2 = boardEntry.getChildEntry("innerSquareColor").getColor();
	mw::Color color3 = boardEntry.getChildEntry("startAreaColor").getColor();
	mw::Color color4 = boardEntry.getChildEntry("playerAreaColor").getColor();
	borderColor_ = boardEntry.getChildEntry("borderColor").getColor();

	auto spriteEntry = boardEntry.getChildEntry("sprites");
	borderHorizontal_ = spriteEntry.getChildEntry("borderHorizontal").getSprite();
	borderVertical_ = spriteEntry.getChildEntry("borderVertical").getSprite();
	borderLeftUp_ = spriteEntry.getChildEntry("borderLeftUp").getSprite();
	borderRightUp_ = spriteEntry.getChildEntry("borderRightUp").getSprite();
	borderDownLeft_ = spriteEntry.getChildEntry("borderDownLeft").getSprite();
	borderDownRight_ = spriteEntry.getChildEntry("borderDownRight").getSprite();
	
	squareSize_ = boardEntry.getChildEntry("squareSize").getFloat();
	sizeBoard_ = squareSize_ * tetrisBoard.getColumns();
	borderSize_ = boardEntry.getChildEntry("borderSize").getFloat();

	initStaticVbo(color1, color2, color3, color4, tetrisBoard.getColumns(), tetrisBoard.getRows());
}

void StaticGraphicBoard::initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows) {
	const float sizeInfo = 100;
	const float sizePlayer = lowX_ + sizeBoard_ + sizeInfo;
	int index = 0;

	width_ = squareSize_ * columns + sizeBoard_ + borderSize_ * 2;
	height_ = squareSize_ * (rows - 2) + lowY_ * 2 + +borderSize_ * 2;

	// 9 [floats/vertices] * 6 [vertices/square] * ((rows - 2) * columns + 3*4) [squares]. 
	std::vector<GLfloat> data(9 * 6 * ((rows - 2)*columns + 3*4)); // Hide the highest 2.
	// Draw the player area.
	float x = lowX_ + borderSize_;
	float y = lowY_ * 0.5f + borderSize_;
	addSquare(data.data(), index,
		x, y,
		sizeBoard_ + sizeInfo + 2 * lowX_, squareSize_ * (rows - 2) + lowY_,
		c4);

	// Draw the outer square.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX_ + borderSize_ + squareSize_ * column + (squareSize_ * columns + sizeInfo) * 0 + squareSize_ * 0.1f;
			y = lowY_ + borderSize_ + squareSize_ * row + squareSize_ * 0.1f;
			addSquare(data.data(), index,
				x, y,
				squareSize_ * 0.8f, squareSize_ * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4);
	addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * 2,
		c3);

	// Draw the preview block area.
	x = lowX_ + borderSize_ + sizeBoard_ + 5;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4) - (squareSize_ * 5 + 5);
	addSquare(data.data(), index,
		x, y,
		squareSize_ * 5, squareSize_ * 5,
		c3);

	// Add border.
	// Left-up corner.
	x = lowX_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderLeftUp_,
		borderColor_);

	// Right-up corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderRightUp_,
		borderColor_);

	// Left-down corner.
	x = lowX_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownLeft_,
		borderColor_);

	// Right-down corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownRight_,
		borderColor_);

	// Up.
	x = lowX_ + borderSize_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Down.
	x = lowX_ + borderSize_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Left.
	x = lowX_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical_,
		borderColor_);

	// Right.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical_,
		borderColor_);

	staticVbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
}

void StaticGraphicBoard::draw(float deltaTime, const BoardShader& shader) {
	if (staticVbo_.getSize() > 0) {
		staticVbo_.bindBuffer();
		setVertexAttribPointer(shader);

		// Draw the static part.
		int nbr = staticVbo_.getSize() / 9 / sizeof(GLfloat);
		mw::glDrawArrays(GL_TRIANGLES, 0, staticVbo_.getSize() / 9 / sizeof(GLfloat)); // 9 floats*bytes/vertex.

		staticVbo_.unbindBuffer();

		mw::checkGlError();
	}
}
