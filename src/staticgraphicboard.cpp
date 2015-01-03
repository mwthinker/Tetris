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

	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * ((rows -2) * columns + 4) [squares]. 
	std::vector<GLfloat> data(sizeof(GLfloat) * 9 * 6 * ((rows - 2)*columns + 4)); // Hide the highest 2.
	// Draw the player area.
	float x = lowX_ + borderSize_;
	float y = lowY_ * 0.5f + borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		sizeBoard_ + sizeInfo + 2 * lowX_, squareSize_ * (rows - 2) + lowY_,
		c4);

	// Draw the outer square.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX_ + borderSize_ + squareSize_ * column + (squareSize_ * columns + sizeInfo) * 0 + squareSize_ * 0.1f;
			y = lowY_ + borderSize_ + squareSize_ * row + squareSize_ * 0.1f;
			BoardShader::addSquare(data.data(), index,
				x, y,
				squareSize_ * 0.8f, squareSize_ * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4);
	BoardShader::addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * 2,
		c3);

	// Draw the preview block area.
	x = lowX_ + borderSize_ + sizeBoard_ + 5;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4) - (squareSize_ * 5 + 5);
	BoardShader::addSquare(data.data(), index,
		x, y,
		squareSize_ * 5, squareSize_ * 5,
		c3);

	// Add border.
	// Left-up corner.
	x = lowX_;
	y = lowY_ + height_ - borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderLeftUp_,
		borderColor_);

	// Right-up corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + height_ - borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderRightUp_,
		borderColor_);

	// Left-down corner.
	x = lowX_;
	y = lowY_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownLeft_,
		borderColor_);

	// Right-down corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownRight_,
		borderColor_);

	// Up.
	x = lowX_ + borderSize_;
	y = lowY_ + height_ - borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Down.
	x = lowX_ + borderSize_;
	y = lowY_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Left.
	x = lowX_;
	y = lowY_ + borderSize_;
	BoardShader::addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical_,
		borderColor_);

	// Right.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + borderSize_;
	BoardShader::addSquare(data.data(), index,
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
		mw::glDrawArrays(GL_TRIANGLES, 0, staticVbo_.getSize() / 9); // 9 vertices/triangle. 

		staticVbo_.unbindBuffer();

		mw::checkGlError();
	}
}

void StaticGraphicBoard::setVertexAttribPointer(const BoardShader& shader) {
	shader.setGlVerA(sizeof(GLfloat) * 9, (GLvoid*) 0);
	shader.setGlTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
	shader.setGlIsTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));
	shader.setGlColorA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
}
