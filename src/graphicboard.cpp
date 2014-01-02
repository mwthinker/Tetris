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

	const mw::Color WHITE(1,1,1,1);

	// Only usefull for the texture in (textureSquares) in defined in gameSprite.h.
	// Sdl flips all images uppside down.
	inline void textureCoord(BlockType blockType, int row, int column) {
		int x = 0;
		int y = 0;
		switch (blockType) {
		case BlockType::I:
			x = 1;
			y = 0;
			break;
		case BlockType::J:
			x = 3;
			y = 0;
			break;
		case BlockType::L:
			x = 1;
			y = 1;
			break;
		case BlockType::O:
			x = 0;
			y = 0;
			break;
		case BlockType::S:
			x = 2;
			y = 0;
			break;
		case BlockType::T:
			x = 0;
			y = 1;
			break;
		case BlockType::Z:
			x = 2;
			y = 1;
			break;
		default:
			// Draw nothing!
			return;
		}
		glTexCoord2f(x / 4.f, y / 2.f);
		glVertex2d(0 + column, 0 + row);
		glTexCoord2f( (x + 1) / 4.f, y / 2.f);
		glVertex2d(1 + column, 0 + row);
		glTexCoord2f( (x + 1) / 4.f, (y + 1) / 2.f);
		glVertex2d(1 + column, 1 + row);
		glTexCoord2f( x / 4.f, (y + 1) / 2.f);
		glVertex2d(0 + column, 1 + row);
	}

	void drawFrame(double x1, double y1, double x2, double y2, double width) {
		glBegin(GL_QUADS);
		glVertex2d(x1, y1);
		glVertex2d(x2, y1);
		glVertex2d(x2, y1 + width);
		glVertex2d(x1, y1 + width);

		glVertex2d(x2 - width, y1);
		glVertex2d(x2,         y1);
		glVertex2d(x2,         y2);
		glVertex2d(x2 - width, y2);

		glVertex2d(x1, y2 - width);
		glVertex2d(x2, y2 - width);
		glVertex2d(x2, y2);
		glVertex2d(x1, y2);

		glVertex2d(x1,         y1);
		glVertex2d(x1 + width, y1);
		glVertex2d(x1 + width, y2);
		glVertex2d(x1,         y2);
		glEnd();
	}

} // Anonymous namespace.

GraphicBoard::GraphicBoard() : tetrisBoard_(20, 10, BlockType::I, BlockType::I) {
	height_ = 800; // Default height for the board.
	voidHeight_ = 10;
	borderLineThickness_ = 7; // Should be lower than voidHeight_.
	previwBorderSizeInSquares_ = 5;
	horizontalDistanceToText_ = 10;

	frameColor_ = mw::Color(237/256.0,78/256.0,8/256.0); // Some type of orange.

	// Define all text sizes and font usage.
	name_ = mw::Text("Player", fontDefault30, 40);
	level_ = mw::Text("Level 1", fontDefault30, 30);
	points_ = mw::Text("Points 0", fontDefault30, 30);
	nbrOfClearedRows_ = mw::Text("Rows 0", fontDefault30, 30);
	middleMessage_ = mw::Text("", fontDefault30, 70);
}

void GraphicBoard::update(int rowsCleared, int points, int level, std::string name) {
	std::stringstream stream;
	stream << "Rows " << rowsCleared;
	nbrOfClearedRows_.setText(stream.str());
	stream.str("");
	stream << "Points " << points;
	points_.setText(stream.str());
	stream.str("");
	stream << "Level " << level;
	level_.setText(stream.str());
	stream.str("");
	name_.setText(name);
}

void GraphicBoard::setMiddleMessage(std::string message) {
	middleMessage_.setText(message);
}

void GraphicBoard::update(const RawTetrisBoard& newBoard) {
	tetrisBoard_ = newBoard;
}

void GraphicBoard::draw() {
	// Update current dimensions.
	rows_ = tetrisBoard_.getNbrOfRows();
	columns_ = tetrisBoard_.getNbrOfColumns();
	pixlePerSquare_ = height_ / (tetrisBoard_.getNbrOfRows() + 2);
	
	glPushMatrix();
	glTranslated(voidHeight_,voidHeight_,0.0);
	drawBoard(tetrisBoard_);
	drawInfo(tetrisBoard_);
	glPopMatrix();
	
	glPushMatrix();
	// Put it in the middle.
	glTranslated(getWidth()*0.5-middleMessage_.getWidth()*0.5,getHeight()*0.5-middleMessage_.getHeight()*0.5,0.0);
	WHITE.glColor3d();
	middleMessage_.draw();
	glPopMatrix();
	
	drawBorder();
}

double GraphicBoard::getWidth() const {
	if (previwBorderSizeInSquares_*pixlePerSquare_ > name_.getWidth()) {
		return columns_ * pixlePerSquare_ + horizontalDistanceToText_ + previwBorderSizeInSquares_*pixlePerSquare_ + voidHeight_*2;
	} else {
		return columns_ * pixlePerSquare_ + horizontalDistanceToText_ + name_.getWidth() + voidHeight_*2;
	}
}

double GraphicBoard::getHeight() const {
	return height_ + voidHeight_*2;
}

void GraphicBoard::drawBorder() const {
	glPushMatrix();
	frameColor_.glColor4d();
	drawFrame(0,0,getWidth(),getHeight(), borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawInfo(const RawTetrisBoard& tetrisBoard) {
	glPushMatrix();
	glTranslated(pixlePerSquare_ * columns_ + horizontalDistanceToText_ + voidHeight_,0,0);
	glTranslated(0,getHeight() - name_.getCharacterSize() - 30,0);

	WHITE.glColor4d();
	name_.draw();

	glPushMatrix();
	glTranslated(80, -180,0);
	drawPreviewBlock(tetrisBoard.nextBlock());
	glPopMatrix();

	glTranslated(0,-650,0);
	WHITE.glColor4d();
	level_.draw();
	glTranslated(0,-40,0);

	points_.draw();
	glTranslated(0,-40,0);

	nbrOfClearedRows_.draw();
	glPopMatrix();
}

void GraphicBoard::drawPreviewBlock(const Block& block) {
	glPushMatrix();

	double x = 0, y = 0;
	for (const Square& sq : block) {
		x += sq.column_;
		y += sq.row_;
	}
	x /= 1.0 * block.nbrOfSquares();
	y /= 1.0 * block.nbrOfSquares();

	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);
	glTranslated(-x - 0.5,-y - 0.5,0);
	drawBlock(block, rows_ + 100);
	glPopMatrix();
	glPushMatrix();
	// Size is 5 squares.
	glScaled(previwBorderSizeInSquares_*pixlePerSquare_,previwBorderSizeInSquares_*pixlePerSquare_,1);
	frameColor_.glColor4d();
	// Makes the line thickness to borderLineThickness_.
	drawFrame(-0.5,-0.5,0.5,0.5,1.0/(previwBorderSizeInSquares_*pixlePerSquare_)*borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawBoard(const RawTetrisBoard& tetrisBoard) {
	glPushMatrix();
	glScaled(pixlePerSquare_, pixlePerSquare_, 1.0);

	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	int rows = rows_ + 2;
	int columns = columns_;
		
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

	glDisable(GL_BLEND);

	// Draw squares.
	glEnable(GL_TEXTURE_2D);
	WHITE.glColor3d();
	textureSquares->bind();
	glBegin(GL_QUADS);
	for (int row = 0; row < rows; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			textureCoord(type, row, column);
		}
	}
	glEnd();
	glDisable(GL_TEXTURE_2D);

	drawBlock(tetrisBoard.currentBlock(), tetrisBoard.getNbrOfRows() + 2);
	drawBeginArea();

	glPopMatrix();
}

void GraphicBoard::drawBeginArea() const {
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	
	glTranslated(0, rows_, 0);
	glScaled(columns_, 2, 0);
	// Draws the outer square
	double red = 0.8, green = 0.2, blue = 0.3;
	glColor4d(red*0.8, green*0.8, blue*0.8, 0.5);
	glBegin(GL_QUADS);
	glVertex2d(0.0,0.0);
	glVertex2d(1.0,0.0);
	glVertex2d(1.0,1.0);
	glVertex2d(0.0,1.0);
	glEnd();

	glPopMatrix();

	glDisable(GL_BLEND);
}

void GraphicBoard::drawBlock(const Block& block, int maxRow) {
	glEnable(GL_TEXTURE_2D);
	
	WHITE.glColor4d();
	textureSquares->bind();
	glBegin(GL_QUADS);
	for (const Square& sq : block) {
		if (sq.row_ < maxRow) {
			textureCoord(block.blockType(), sq.row_, sq.column_);
		}
	}
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
}
