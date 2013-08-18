#include "graphicboard.h"
#include "tetrisboard.h"
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

	void bindTexture(BlockType blockType) {
		switch (blockType) {
		case BlockType::I:
			textureI->bind();
			break;
		case BlockType::J:
			textureJ->bind();
			break;
		case BlockType::L:
			textureL->bind();
			break;
		case BlockType::O:
			textureO->bind();
			break;
		case BlockType::S:
			textureS->bind();
			break;
		case BlockType::T:
			textureT->bind();
			break;
		case BlockType::Z:
			textureZ->bind();
			break;
		default:
			break;
		}
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
}

GraphicBoard::GraphicBoard() {
	height_ = 800; // height for the board
	voidHeight_ = 10;
	borderLineThickness_ = 7; // Should be lower than voidHeight_
	previwBorderSizeInSquares_ = 5;
	horizontalDistanceToText_ = 40;

	frameColor_ = mw::Color(237/256.0,78/256.0,8/256.0);

	name_ = mw::Text("Player", fontDefault, 40);
	level_ = mw::Text("Level 1", fontDefault, 30);
	points_ = mw::Text("Points 0", fontDefault, 30);
	nbrOfClearedRows_ = mw::Text("Rows 0", fontDefault, 30);
	middleMessage_ = mw::Text("", fontDefault, 70);
}

void GraphicBoard::setNbrOfClearedRows(int nbr) {
	std::stringstream stream;
	stream << "Rows " << nbr;
	nbrOfClearedRows_.setText(stream.str());
}

void GraphicBoard::setPoints(int points) {
	std::stringstream stream;
	stream << "Points " << points;
	points_.setText(stream.str());
}

void GraphicBoard::setLevel(int level) {
	std::stringstream stream;
	stream << "Level " << level;
	level_.setText(stream.str());
}

void GraphicBoard::setName(std::string name) {
	name_.setText(name);
}

void GraphicBoard::setMiddleMessage(std::string message) {
	middleMessage_.setText(message);
}

void GraphicBoard::draw(const TetrisBoard& tetrisBoard) {
	// Update current dimensions.
	rows_ = tetrisBoard.getNbrOfRows();
	columns_ = tetrisBoard.getNbrOfColumns();

    pixlePerSquare_ = height_ / (tetrisBoard.getNbrOfRows() + 2);
	glPushMatrix();
	glTranslated(voidHeight_,voidHeight_,0.0);
	drawBoard(tetrisBoard);
	drawInfo(tetrisBoard);
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

// Private
void GraphicBoard::drawBorder() const {
	glPushMatrix();
	frameColor_.glColor4d();
	drawFrame(0,0,getWidth(),getHeight(), borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawInfo(const TetrisBoard& tetrisBoard) {
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

void GraphicBoard::drawBoard(const TetrisBoard& tetrisBoard) {
	glPushMatrix();
	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);

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
	WHITE.glColor4d();
	glEnable(GL_TEXTURE_2D);
	for (int row = 0; row < rows; ++row) {
		for (int column = 0; column < columns; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			if (type != BlockType::EMPTY) {
				bindTexture(type);
				glBegin(GL_QUADS);
				glTexCoord2d(0, 0);
				glVertex2d(0 + column, 0 + row);
				glTexCoord2d(1, 0);
				glVertex2d(1 + column, 0 + row);
				glTexCoord2d(1, 1);
				glVertex2d(1 + column, 1 + row);
				glTexCoord2d(0, 1);
				glVertex2d(0 + column, 1 + row);
				glEnd();
			}
		}
	}	
	glDisable(GL_TEXTURE_2D);

	drawBlock(tetrisBoard.currentBlock(), tetrisBoard.getNbrOfRows() + 2);
	drawBeginArea();

	glPopMatrix();
}

void GraphicBoard::drawBeginArea() const {
	double red = 0.8, green = 0.2, blue = 0.3;

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslated(0, rows_, 0);
	glScaled(columns_, 2, 0);
	// Draws the outer square
	glColor4d(red*0.8,green*0.8,blue*0.8,0.5);
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
	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_2D);
	
	WHITE.glColor4d();
	bindTexture(block.blockType());
	glBegin(GL_QUADS);
	for (const Square& sq : block) {
		if (sq.row_ < maxRow) {
			glTexCoord2d(0, 0);
			glVertex2d(0 + sq.column_, 0 + sq.row_);
			glTexCoord2d(1, 0);
			glVertex2d(1 + sq.column_, 0 + sq.row_);
			glTexCoord2d(1, 1);
			glVertex2d(1 + sq.column_, 1 + sq.row_);
			glTexCoord2d(0, 1);
			glVertex2d(0 + sq.column_, 1 + sq.row_);
		}
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
}
