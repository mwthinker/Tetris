#include "staticgraphicboard.h"
#include "rawtetrisboard.h"
#include "boardshader.h"

#include <mw/opengl.h>
#include <mw/color.h>
#include <mw/sprite.h>

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
		const mw::Sprite& sprite, mw::Color color = mw::Color(1, 1, 1)) {
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

StaticGraphicBoard::StaticGraphicBoard(float lowX, float lowY,
	TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) {

	const mw::Color c1 = boardEntry.getChildEntry("outerSquareColor").getColor();
	const mw::Color c2 = boardEntry.getChildEntry("innerSquareColor").getColor();
	const mw::Color c3 = boardEntry.getChildEntry("startAreaColor").getColor();
	const mw::Color c4 = boardEntry.getChildEntry("playerAreaColor").getColor();
	const mw::Color borderColor = boardEntry.getChildEntry("borderColor").getColor();

	auto spriteEntry = boardEntry.getChildEntry("sprites");
	const mw::Sprite borderHorizontal = spriteEntry.getChildEntry("borderHorizontal").getSprite();
	const mw::Sprite borderVertical = spriteEntry.getChildEntry("borderVertical").getSprite();
	const mw::Sprite borderLeftUp = spriteEntry.getChildEntry("borderLeftUp").getSprite();
	const mw::Sprite borderRightUp = spriteEntry.getChildEntry("borderRightUp").getSprite();
	const mw::Sprite borderDownLeft = spriteEntry.getChildEntry("borderDownLeft").getSprite();
	const mw::Sprite borderDownRight = spriteEntry.getChildEntry("borderDownRight").getSprite();
	
	const float squareSize = boardEntry.getChildEntry("squareSize").getFloat();
	borderSize_ = boardEntry.getChildEntry("borderSize").getFloat();

	const int columns = tetrisBoard.getColumns();
	const int rows = tetrisBoard.getRows();

	const float infoSize = 70;
	boardWidth_ = squareSize * columns;

	width_ = squareSize * columns + infoSize + borderSize_ * 2;
	height_ = squareSize * (rows - 2) + borderSize_ * 2;

	// 9 [floats/vertices] * 6 [vertices/square] * ((rows - 2) * columns + 3*4) [squares].
	std::vector<GLfloat> data(9 * 6 * ((rows - 2)*columns + 3 * 4)); // Hide the highest 2.
	int index = 0;

	// Draw the player area.
	float x = lowX + borderSize_;
	float y = lowY * 0.5f + borderSize_;
	addSquare(data.data(), index,
		x, y,
		boardWidth_ + infoSize, squareSize * (rows - 2),
		c4);

	// Draw the outer square.
	x = lowX + borderSize_;
	y = lowY + borderSize_;
	addSquare(data.data(), index,
		x, y,
		squareSize * columns, squareSize * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + borderSize_ + squareSize * column + (squareSize * columns + infoSize) * 0 + squareSize * 0.1f;
			y = lowY + borderSize_ + squareSize * row + squareSize * 0.1f;
			addSquare(data.data(), index,
				x, y,
				squareSize * 0.8f, squareSize * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX + borderSize_;
	y = lowY + borderSize_ + squareSize * (rows - 4);
	addSquare(data.data(), index,
		x, y,
		squareSize * columns, squareSize * 2,
		c3);

	// Draw the preview block area.
	x = lowX + borderSize_ + boardWidth_ + 5;
	y = lowY + borderSize_ + squareSize * (rows - 4) - (squareSize * 5 + 5);
	addSquare(data.data(), index,
		x, y,
		squareSize * 5, squareSize * 5,
		c3);

	// Add border.
	// Left-up corner.
	x = lowX;
	y = lowY + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderLeftUp,
		borderColor);

	// Right-up corner.
	x = lowX + width_ - borderSize_;
	y = lowY + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderRightUp,
		borderColor);

	// Left-down corner.
	x = lowX;
	y = lowY;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownLeft,
		borderColor);

	// Right-down corner.
	x = lowX + width_ - borderSize_;
	y = lowY;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownRight,
		borderColor);

	// Up.
	x = lowX + borderSize_;
	y = lowY + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal,
		borderColor);

	// Down.
	x = lowX + borderSize_;
	y = lowY;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal,
		borderColor);

	// Left.
	x = lowX;
	y = lowY + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical,
		borderColor);

	// Right.
	x = lowX + width_ - borderSize_;
	y = lowY + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical,
		borderColor);

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

float StaticGraphicBoard::getWidth() const {
	return width_;
}

float StaticGraphicBoard::getHeight() const {
	return height_;
}

float StaticGraphicBoard::getBoardWidth() const {
	return boardWidth_;
}

float StaticGraphicBoard::getBorderSize() const {
	return borderSize_;
}
