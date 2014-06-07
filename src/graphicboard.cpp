#include "graphicboard.h"
#include "tetrisboard.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>

#include <SDL_opengl.h>
#include <sstream>

namespace {

	const mw::Color WHITE(1, 1, 1, 1);

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

} // Anonymous namespace.

GraphicPlayerInfo::GraphicPlayerInfo(const mw::Font& font) {
	// Define all text sizes and font usage.
	level_ = mw::Text("", font, 14);
	points_ = mw::Text("", font, 14);
	nbrOfClearedRows_ = mw::Text("", font, 14);
	showPoints_ = true;
	font_ = font;
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
	glPushMatrix(); // 1
	WHITE.glColor4d();
	nbrOfClearedRows_.draw();
	glTranslated(0, nbrOfClearedRows_.getCharacterSize() + 2, 0);
	if (showPoints_) {
		points_.draw();
		glTranslated(0, nbrOfClearedRows_.getCharacterSize() + 2, 0);
	}

	level_.draw();

	glPopMatrix(); // 1
}

float GraphicPlayerInfo::getWidth() const {
	return 150;
}

float GraphicPlayerInfo::getHeight() const {
	return 150;
}

GraphicPreviewBlock::GraphicPreviewBlock() {
}

void GraphicPreviewBlock::draw() {
	glPushMatrix(); // 1

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
	glTranslated(-x, -y, 0);
	
	const mw::Sprite& sprite = getSprite(block_.blockType());
	for (const Square& sq : block_) {
		glPushMatrix();
		glTranslated(sq.column_, sq.row_, 0);
		sprite.draw();
		glPopMatrix();
	}
	
	glPopMatrix(); // 1
}

void GraphicPreviewBlock::update(const BlockType& blockType, float pixlePerSquare) {
	block_ = Block(blockType, 100, 100);
	pixlePerSquare_ = pixlePerSquare;
	width_ = 5 * pixlePerSquare_;
	height_ = 5 * pixlePerSquare_;
}

float GraphicPreviewBlock::getWidth() const {
	return width_;
}

float GraphicPreviewBlock::getHeight() const {
	return height_;
}

GraphicBoard::GraphicBoard() : tetrisBoard_(nullptr) {
}

GraphicBoard::GraphicBoard(const TetrisBoard& tetrisBoard) : tetrisBoard_(&tetrisBoard) {
	height_ = 800; // Default height for the board.
	frameColor_ = mw::Color(237 / 256.0, 78 / 256.0, 8 / 256.0); // Some type of orange.
}

void GraphicBoard::update() {
	height_ = 400;
	pixlePerSquare_ = height_ / (tetrisBoard_->getNbrOfRows() + 2);
	width_ = tetrisBoard_->getNbrOfColumns() * pixlePerSquare_;
}

void GraphicBoard::draw() {
	drawBoard(*tetrisBoard_);
}

float GraphicBoard::getWidth() const {
	return width_;
}

float GraphicBoard::getHeight() const {
	return height_;
}

void GraphicBoard::drawBoard(const RawTetrisBoard& tetrisBoard) const {
	glPushMatrix();  // 1.
	glScaled(pixlePerSquare_, pixlePerSquare_, 1.0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);	
	int rows = tetrisBoard.getNbrOfRows();
	int columns = tetrisBoard.getNbrOfColumns();

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
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	
	drawBeginArea();

	glPushMatrix();  // 2.
	glTranslated(0.5, 0.5, 0); // Sprite is [-0.5, 0.5] in y and x direction.

	WHITE.glColor3d();
	// Draw board.
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			const mw::Sprite& sprite = getSprite(type);
			glPushMatrix(); // 3.
			glTranslated(column, row, 0);
			sprite.draw();
			glPopMatrix(); // 3.
		}
	}
	
	// Draw block.
	const mw::Sprite& sprite = getSprite(tetrisBoard.getBlockType());
	for (const Square& sq : tetrisBoard.getBlock()) {
		if (sq.row_ < tetrisBoard.getNbrOfRows() + 2) {
			glPushMatrix();
			glTranslated(sq.column_, sq.row_, 0);
			sprite.draw();
			glPopMatrix();
		}
	}

	glPopMatrix();  // 2.
	glPopMatrix();  // 1.
}

void GraphicBoard::drawBeginArea() const {
	glPushMatrix(); // 1.

	int rows = tetrisBoard_->getNbrOfRows();
	int columns = tetrisBoard_->getNbrOfColumns();
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

	glPopMatrix(); // 1.
}
