#include "gamegraphic.h"
#include "tetrisboard.h"
#include "tetrisentry.h"

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

	const mw::Color WHITE(1, 1, 1, 1);

	void drawFrame(double x1, double y1, double x2, double y2, double width) {
		/*
		glBegin(GL_QUADS);
		glVertex2d(x1, y1);
		glVertex2d(x2, y1);
		glVertex2d(x2, y1 + width);
		glVertex2d(x1, y1 + width);

		glVertex2d(x2 - width, y1);
		glVertex2d(x2, y1);
		glVertex2d(x2, y2);
		glVertex2d(x2 - width, y2);

		glVertex2d(x1, y2 - width);
		glVertex2d(x2, y2 - width);
		glVertex2d(x2, y2);
		glVertex2d(x1, y2);

		glVertex2d(x1, y1);
		glVertex2d(x1 + width, y1);
		glVertex2d(x1 + width, y2);
		glVertex2d(x1, y2);
		glEnd();
		*/
	}

} // Anonymous namespace.

GraphicPlayerInfo::GraphicPlayerInfo(const mw::Font& font) {
	// Define all text sizes and font usage.
	level_ = mw::Text("", font, 14);
	points_ = mw::Text("", font, 14);
	nbrOfClearedRows_ = mw::Text("", font, 14);
	showPoints_ = true;
	font_ = font;
}

namespace {

	inline void addVertex(std::vector<GLfloat>& data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		mw::Color color) {
		data[++index] = x;
		data[++index] = y;

		data[++index] = xTex;
		data[++index] = yTex;

		data[++index] = isTex;

		data[++index] = color.red_;
		data[++index] = color.green_;
		data[++index] = color.blue_;
		data[++index] = color.alpha_;
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(std::vector<GLfloat>& data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		mw::Color color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(std::vector<GLfloat>& data, int& index,
		float x, float y,
		float w, float h,
		mw::Color color) {

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

	inline void addSquare(std::vector<GLfloat>& data, int& index,
		float x, float y,
		float w, float h,
		mw::Sprite& sprite) {
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
			mw::Color(1, 1, 1));
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
			mw::Color(1, 1, 1));
	}

	mw::Sprite getBoardSprite(mw::Texture texture, TetrisEntry spriteEntry) {
		float x = spriteEntry.getChildEntry("x").getFloat();
		float y = spriteEntry.getChildEntry("y").getFloat();
		float w = spriteEntry.getChildEntry("w").getFloat();
		float h = spriteEntry.getChildEntry("h").getFloat();
		return mw::Sprite(texture, x, y, w, h);
	}

	void setVertexAttribPointer(const mw::Shader& shader) {
		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aPos"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aPos"), 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) 0);

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aTex"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aTex"), 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aIsTex"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aIsTex"), 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aColor"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
	}

	gui::Point calculateCenter(const Block& block) {
		gui::Point point(0, 0);
		for (const Square& sq : block) {
			point.x_ += sq.column_;
			point.y_ += sq.row_;
		}

		point.x_ /= block.nbrOfSquares();
		point.y_ /= block.nbrOfSquares();

		return point;
	}

}

GameGraphic::GameGraphic() {
}

void GameGraphic::initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows) {
	const float lowX = 20;
	const float lowY = 20;

	const float sizeBetweenPlayers = 10;
	const float sizeInfo = 100;
	const float sizePlayer = lowX + sizeBoard_ + sizeInfo;
	vertercies_ = 0;
	int index = -1;

	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 4) [squares]. 
	std::vector<GLfloat> data(sizeof(GLfloat) * 9 * 6 * ((rows + 2)*columns + 4));
	// Draw the player area.
	float x = (sizeBoard_ + sizeBetweenPlayers) * 0;
	float y = lowY * 0.5f;
	addSquare(data, index,
		x, y,
		sizeBoard_ + sizeInfo + 2 * lowX, squareSize_ * (rows + 2) + lowY,
		c4);
	vertercies_ += 6;

	// Draw the outer square.
	x = lowX + 0 * (squareSize_ * columns + sizeInfo + sizeBetweenPlayers);
	y = lowY;
	addSquare(data, index,
		x, y,
		squareSize_ * columns, squareSize_ * (rows + 2),
		c1);
	vertercies_ += 6;

	indexBoard_ = data.size();

	// Draw the inner squares.
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + squareSize_ * column + (squareSize_ * columns + sizeInfo) * 0 + squareSize_ * 0.1f;
			y = lowY + squareSize_ * row + squareSize_ * 0.1f;
			addSquare(data, index,
				x, y,
				squareSize_ * 0.8f, squareSize_ * 0.8f,
				c2);
			vertercies_ += 6;
		}
	}

	// Draw the block start area.
	x = lowX;
	y = lowY + squareSize_ * rows;
	addSquare(data, index,
		x, y,
		squareSize_ * columns, squareSize_ * 2,
		c3);
	vertercies_ += 6;

	// Draw the preview block area.
	x = lowX + sizeBoard_ + 5;
	y = lowY + squareSize_ * rows - (squareSize_ * 5 + 5);
	addSquare(data, index,
		x, y,
		squareSize_ * 5, squareSize_ * 5,
		c3);
	vertercies_ += 6;

	width_ = squareSize_ * columns + (sizeBoard_ + sizeBetweenPlayers) * (1 - 1) + lowX * 2;
	height_ = squareSize_ * (rows + 2) + lowY * 2;

	staticVbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
}

void GameGraphic::initDynamicVbo(const RawTetrisBoard& tetrisBoard) {
	updateDynamicData(tetrisBoard);
	vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dynamicData_.size(), dynamicData_.data(), GL_DYNAMIC_DRAW);
}

void GameGraphic::updateDynamicData(const RawTetrisBoard& tetrisBoard) {
	const float lowX = 20;
	const float lowY = 20;

	const int rows = tetrisBoard.getRows();
	const int columns = tetrisBoard.getColumns();

	dynamicVertercies_ = 0;
	int index = -1;

	// Draw the board.
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			if (type != BlockType::EMPTY && type != BlockType::WALL) {
				float x = lowX + squareSize_ * column;
				float y = lowY + squareSize_ * row;
				mw::Sprite sprite = getSprite(type);
				addSquare(dynamicData_, index,
					x, y,
					squareSize_, squareSize_,
					sprite);
				dynamicVertercies_ += 6;
			}
		}
	}

	// Draw the current block.
	float x = lowX;
	float y = lowY;
	Block block = tetrisBoard.getBlock();
	mw::Sprite sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		if (sq.row_ < rows + 2) {
			addSquare(dynamicData_, index,
				x + sq.column_ * squareSize_, y + sq.row_ * squareSize_,
				squareSize_, squareSize_,
				sprite);
			dynamicVertercies_ += 6;
		}
	}

	// Draw the preview block.
	x = lowX + sizeBoard_ + 5 + squareSize_ * 2.5f;
	y = lowY + squareSize_ * rows - (squareSize_ * 2.5f + 5);
	block = Block(tetrisBoard.getNextBlockType(), 0, 0);
	gui::Point center = calculateCenter(block);
	sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		addSquare(dynamicData_, index,
			x + (center.x_ - sq.column_ - 0.5f) * squareSize_, y + (center.y_ - sq.row_ - 0.5f) * squareSize_,
			squareSize_, squareSize_,
			sprite);
		dynamicVertercies_ += 6;
	}
}

GameGraphic::GameGraphic(TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares]. 
dynamicData_(sizeof(GLfloat) * 9 * 6 * (tetrisBoard.getRows() * tetrisBoard.getColumns() + 8)) {

	mw::Color color1 = boardEntry.getChildEntry("outerSquareColor").getColor();
	mw::Color color2 = boardEntry.getChildEntry("innerSquareColor").getColor();
	mw::Color color3 = boardEntry.getChildEntry("startAreaColor").getColor();
	mw::Color color4 = boardEntry.getChildEntry("playerAreaColor").getColor();

	mw::Texture texture = boardEntry.getChildEntry("texture").getTexture();
	spriteZ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareZ"));
	spriteS_ = getBoardSprite(texture, boardEntry.getChildEntry("squareS"));
	spriteJ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareJ"));
	spriteI_ = getBoardSprite(texture, boardEntry.getChildEntry("squareI"));
	spriteL_ = getBoardSprite(texture, boardEntry.getChildEntry("squareL"));
	spriteT_ = getBoardSprite(texture, boardEntry.getChildEntry("squareT"));
	spriteO_ = getBoardSprite(texture, boardEntry.getChildEntry("squareO"));

	squareSize_ = 10;
	sizeBoard_ = squareSize_ * tetrisBoard.getColumns();

	initStaticVbo(color1, color2, color3, color4, tetrisBoard.getColumns(), tetrisBoard.getRows());
	initDynamicVbo(tetrisBoard);
}

void GameGraphic::update(const PlayerPtr& player) {
	updateDynamicData(player->getTetrisBoard());
	vbo_.bindBuffer();
	mw::glBufferSubData(vbo_.getTarget(), 0, sizeof(GLfloat) * dynamicVertercies_ * 9, dynamicData_.data());
	vbo_.unbindBuffer();
}

void GameGraphic::draw(mw::Shader& shader) {
	if (staticVbo_.getSize() > 0) {
		staticVbo_.bindBuffer();
		setVertexAttribPointer(shader);

		// Draw the static part.
		mw::glDrawArrays(GL_TRIANGLES, 0, vertercies_);

		staticVbo_.unbindBuffer();

		// Draw the dynamic part.
		if (vbo_.getSize() > 0) {
			vbo_.bindBuffer();
			spriteI_.bindTexture(); // All sprites uses the same texture.
			setVertexAttribPointer(shader);

			mw::glDrawArrays(GL_TRIANGLES, 0, dynamicVertercies_);

			vbo_.unbindBuffer();
		}

		mw::checkGlError();
	}
}

mw::Sprite GameGraphic::getSprite(BlockType blockType) const {
	switch (blockType) {
		case BlockType::I:
			return spriteI_;
		case BlockType::J:
			return spriteJ_;
		case BlockType::L:
			return spriteL_;
		case BlockType::O:
			return spriteO_;
		case BlockType::S:
			return spriteS_;
		case BlockType::T:
			return spriteT_;
		case BlockType::Z:
			return spriteZ_;
	}
	assert(0); // Should not be here.
	return mw::Sprite();
}

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GraphicPlayerInfo::update(int rowsCleared, int points, int level) {
	std::stringstream stream;
	stream << "Rows " << rowsCleared;
	nbrOfClearedRows_.setText(stream.str());
	stream.str("");
	stream << "Points " << points;
	points_.setText(stream.str());
	stream.str("");
	stream << "Level " << level;
	level_.setText(stream.str());
}

void GraphicPlayerInfo::draw(gui::WindowMatrixPtr wp) {
	wp->useShader();
	auto old = wp->getModel();
	wp->setModel(old * mw::getTranslateMatrix44(5, 5));
	wp->setColor(WHITE);
	nbrOfClearedRows_.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(0, nbrOfClearedRows_.getCharacterSize() + 2));
	if (showPoints_) {
		points_.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix44(0, nbrOfClearedRows_.getCharacterSize() + 2));
	}

	level_.draw();

	wp->setModel(old);
}

float GraphicPlayerInfo::getWidth() const {
	return 150;
}

float GraphicPlayerInfo::getHeight() const {
	return 150;
}
