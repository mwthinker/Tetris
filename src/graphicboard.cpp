#include "graphicboard.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisboard.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>

#include <SDL_opengl.h>
#include <sstream>

namespace {

	const mw::Color WHITE(1, 1, 1, 1);

	// Only usefull for the texture in (textureSquares) in defined in gameSprite.h.
	// Sdl flips all images uppside down.
	inline void textureCoord(BlockType blockType, int row, int column) {
		mw::Sprite sprite;
		switch (blockType) {
			case BlockType::I:
				sprite = spriteI;
				break;
			case BlockType::J:
				sprite = spriteJ;
				break;
			case BlockType::L:
				sprite = spriteL;
				break;
			case BlockType::O:
				sprite = spriteO;
				break;
			case BlockType::S:
				sprite = spriteS;
				break;
			case BlockType::T:
				sprite = spriteT;
				break;
			case BlockType::Z:
				sprite = spriteZ;
				break;
			default:
				// Draw nothing!
				return;
		}
		float h = (float) sprite.getTexture().getHeight();
		float w = (float) sprite.getTexture().getWidth();

		glTexCoord2f(sprite.getX() / w, sprite.getY() / h);
		glVertex2d(0 + column, 0 + row);
		glTexCoord2f((sprite.getX() + sprite.getWidth()) / w, sprite.getY() / h);
		glVertex2d(1 + column, 0 + row);
		glTexCoord2f((sprite.getX() + sprite.getWidth()) / w, (sprite.getY() + sprite.getHeight()) / h);
		glVertex2d(1 + column, 1 + row);
		glTexCoord2f(sprite.getX() / w, (sprite.getY() + sprite.getHeight()) / h);
		glVertex2d(0 + column, 1 + row);
	}

	void drawFrame(double x1, double y1, double x2, double y2, double width) {
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
	}

	void drawBlock(const Block& block, int maxRow) {
		WHITE.glColor4d();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);		
		glEnable(GL_TEXTURE_2D);
		textureSquares.bind();
		glBegin(GL_QUADS);
		for (const Square& sq : block) {
			if (sq.row_ < maxRow) {
				textureCoord(block.blockType(), sq.row_, sq.column_);
			}
		}
		glEnd();
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
	}

} // Anonymous namespace.

GraphicPlayerInfo::GraphicPlayerInfo() {
	// Define all text sizes and font usage.
	level_ = mw::Text("", fontDefault30, 14);
	points_ = mw::Text("", fontDefault30, 14);
	nbrOfClearedRows_ = mw::Text("", fontDefault30, 14);
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

void GraphicPlayerInfo::draw() {
	glPushMatrix();
	WHITE.glColor4d();
	nbrOfClearedRows_.draw();
	glTranslated(0, nbrOfClearedRows_.getCharacterSize() + 2, 0);

	points_.draw();
	glTranslated(0, nbrOfClearedRows_.getCharacterSize() + 2, 0);

	level_.draw();

	glPopMatrix();
}

double GraphicPlayerInfo::getWidth() const {
	return 150;
}

double GraphicPlayerInfo::getHeight() const {
	return 150;
}

GraphicPreviewBlock::GraphicPreviewBlock() {
}

void GraphicPreviewBlock::draw() {
	glPushMatrix();

	double x = 0, y = 0;
	for (const Square& sq : block_) {
		x += sq.column_;
		y += sq.row_;
	}

	// Center the blocks.
	glTranslated(width_ * 0.5, height_ * 0.5, 0);

	// Block scale.
	glScaled(pixlePerSquare_, pixlePerSquare_, 1.0);

	// Calculate center of mass
	x /= block_.nbrOfSquares();
	y /= block_.nbrOfSquares();

	// Move center to origo.
	glTranslated(-x - 0.5, -y - 0.5, 0); // -0.5 Due, mass center for a square is 0.5;
	drawBlock(block_, 10000);
	glPopMatrix();
}

void GraphicPreviewBlock::update(const BlockType& blockType, double pixlePerSquare) {
	block_ = Block(blockType, 100, 100);
	pixlePerSquare_ = pixlePerSquare;
	width_ = 5 * pixlePerSquare_;
	height_ = 5 * pixlePerSquare_;
}

double GraphicPreviewBlock::getWidth() const {
	return width_;
}

double GraphicPreviewBlock::getHeight() const {
	return height_;
}

GraphicBoard::GraphicBoard() : tetrisBoard_(20, 10, BlockType::I, BlockType::I) {
	height_ = 800; // Default height for the board.
	frameColor_ = mw::Color(237 / 256.0, 78 / 256.0, 8 / 256.0); // Some type of orange.
}

void GraphicBoard::update(const RawTetrisBoard& newBoard) {
	tetrisBoard_ = newBoard;
	height_ = 400;
	pixlePerSquare_ = height_ / (tetrisBoard_.getNbrOfRows() + 2);
	width_ = tetrisBoard_.getNbrOfColumns() * pixlePerSquare_;
}

void GraphicBoard::draw() {
	drawBoard(tetrisBoard_);
}

double GraphicBoard::getWidth() const {
	return width_;
}

double GraphicBoard::getHeight() const {
	return height_;
}

void GraphicBoard::drawBoard(const RawTetrisBoard& tetrisBoard) const {
	glPushMatrix();
	glScaled(pixlePerSquare_, pixlePerSquare_, 1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	int rows = tetrisBoard_.getNbrOfRows();
	int columns = tetrisBoard_.getNbrOfColumns();

	glBegin(GL_QUADS);
	// Draws the outer square.
	const float red = 0.8f, green = 0.2f, blue = 0.3f;
	glColor4d(red * 0.8, green * 0.8, blue * 0.8, 0.6);
	glVertex2d(0, 0);
	glVertex2d(columns, 0);
	glVertex2d(columns, rows);
	glVertex2d(0, rows);

	// Draws the inner square.
	glColor4d(red * 0.1, green * 0.1, blue * 0.1, 0.1);
	for (int row = 0; row < rows; ++row) {
		for (int column = 0; column < columns; ++column) {
			glVertex2d(0.1 + column, 0.1 + row);
			glVertex2d(0.9 + column, 0.1 + row);
			glVertex2d(0.9 + column, 0.9 + row);
			glVertex2d(0.1 + column, 0.9 + row);
		}
	}
	glEnd();	

	WHITE.glColor4d();

	// Draw squares.
	glEnable(GL_TEXTURE_2D);
	textureSquares.bind();
	glBegin(GL_QUADS);
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			textureCoord(type, row, column);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);

	drawBeginArea();
	drawBlock(tetrisBoard.currentBlock(), tetrisBoard.getNbrOfRows() + 2);

	glPopMatrix();
}

void GraphicBoard::drawBeginArea() const {
	glPushMatrix();

	int rows = tetrisBoard_.getNbrOfRows();
	int columns = tetrisBoard_.getNbrOfColumns();
	glTranslated(0, rows, 0);
	glScaled(columns, 2, 0);
	// Draws the outer square
	double red = 0.8, green = 0.2, blue = 0.3;
	glColor4d(red*0.8, green*0.8, blue*0.8, 0.5);
	glBegin(GL_QUADS);
	glVertex2d(0.0, 0.0);
	glVertex2d(1.0, 0.0);
	glVertex2d(1.0, 1.0);
	glVertex2d(0.0, 1.0);
	glEnd();

	glPopMatrix();
}
