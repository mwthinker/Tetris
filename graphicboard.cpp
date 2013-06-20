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

GraphicBoard::GraphicBoard(const TetrisBoard& tetrisBoard) : tetrisBoard_(tetrisBoard) {
	height_ = 800; // height for the board
	voidHeight_ = 10;
	borderLineThickness_ = 7; // Should be lower than voidHeight_
	previwBorderSizeInSquares_ = 5;
	horizontalDistanceToText_ = 40;

	color_ = mw::Color(237/256.0,78/256.0,8/256.0);

	text1_ = mw::Text("Player",fontDefault);
	text2_ = mw::Text("Level ",fontDefault);
	text3_ = mw::Text("Points",fontDefault);
	text4_ = mw::Text("Rows",fontDefault);

	gameOverMessage_ = mw::Text("", fontDefault,70);
	countDown_ = mw::Text("", fontDefault,70);

	nbrOfClearedRows_ = 0;
	points_ = 0;
	level_ = 0;
	name_ = "Player";
}

GraphicBoard::~GraphicBoard() {
}

void GraphicBoard::setNbrOfClearedRows(int nbr) {
	nbrOfClearedRows_ = nbr;
}

void GraphicBoard::setPoints(int points) {
	points_ = points;
}

void GraphicBoard::setLevel(int level) {
	level_ = level;
}

void GraphicBoard::setName(std::string name) {
	name_ = name;
}

void GraphicBoard::setGameOverMessage(std::string message) {
	gameOverMessage_.setText(message);
}

void GraphicBoard::setCountDownMessage(std::string countDown) {
	countDown_.setText(countDown);
}

void GraphicBoard::draw() {
    pixlePerSquare_ = height_ / (tetrisBoard_.getNbrOfRows() + 2);
	glPushMatrix();
	glTranslated(voidHeight_,voidHeight_,0.0);
	drawBoard();
	drawInfo();
	glPopMatrix();	
	
	glPushMatrix();
	// Put it in the middle.
	glTranslated(getWidth()*0.5-gameOverMessage_.getWidth()*0.5,getHeight()*0.5-gameOverMessage_.getHeight()*0.5,0.0);
	glColor3d(1,1,1);
	if (tetrisBoard_.isGameOver()) {
		gameOverMessage_.draw();
	}
	countDown_.draw();
	glPopMatrix();	

	drawBorder();
}

double GraphicBoard::getWidth() const {
	if (previwBorderSizeInSquares_*pixlePerSquare_ > text1_.getWidth()) {
		return tetrisBoard_.getNbrOfColumns()*pixlePerSquare_ + horizontalDistanceToText_ + previwBorderSizeInSquares_*pixlePerSquare_ + voidHeight_*2;
	} else {
		return tetrisBoard_.getNbrOfColumns()*pixlePerSquare_ + horizontalDistanceToText_ + text1_.getWidth() + voidHeight_*2;
	}
}

double GraphicBoard::getHeight() const {
	return height_ + voidHeight_*2;
}

// Private
void GraphicBoard::drawBorder() const {
	glPushMatrix();
	color_.glColor4d();
	drawFrame(0,0,getWidth(),getHeight(), borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawInfo() {
	glPushMatrix();
	glTranslated(pixlePerSquare_ * tetrisBoard_.getNbrOfColumns() + horizontalDistanceToText_ + voidHeight_,0,0);
	glTranslated(0,getHeight() - text1_.getCharacterSize() - 30,0);
	text1_.setText(name_);
	text1_.setCharacterSize(40);
	glColorWhite();
	text1_.draw();

	glPushMatrix();
	glTranslated(80, -180,0);
	drawPreviewBlock();
	glPopMatrix();

	glTranslated(0,-650,0);
	std::stringstream ss;
	ss << "Level " << level_;
	text2_.setText(ss.str());
	glColorWhite();
	text2_.draw();
	glTranslated(0,-40,0);

	ss.str("");
	ss << "Points " << points_;
	text3_.setText(ss.str());
	text3_.draw();
	glTranslated(0,-40,0);


	ss.str("");
	ss << "Rows " << nbrOfClearedRows_;
	text4_.setText(ss.str());
	text4_.draw();
	glPopMatrix();
}

void GraphicBoard::drawPreviewBlock() {
	glPushMatrix();
	const Block& block = tetrisBoard_.nextBlock();

	int nbrOfSquares = block.nbrOfSquares();
	double x = 0, y = 0;
	for (int i = 0; i < nbrOfSquares; ++i) {
		Square square = block[i];
		x += square.column_ * 1.0 / nbrOfSquares;
		y += square.row_ * 1.0 / nbrOfSquares;
	}

	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);
	glTranslated(-x - 0.5,-y - 0.5,0);
	drawBlock(block, tetrisBoard_.getNbrOfRows() + 100);
	glPopMatrix();
	glPushMatrix();
	// Size is 5 squares.
	glScaled(previwBorderSizeInSquares_*pixlePerSquare_,previwBorderSizeInSquares_*pixlePerSquare_,1);
	color_.glColor4d();
	// Makes the line thickness to borderLineThickness_.
	drawFrame(-0.5,-0.5,0.5,0.5,1.0/(previwBorderSizeInSquares_*pixlePerSquare_)*borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawBoard() {
	glPushMatrix();
	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);

	drawGrid();

	for (int row = 0; row < tetrisBoard_.getNbrOfRows() + 2; ++row) {
		for (int column = 0; column < tetrisBoard_.getNbrOfColumns(); ++column) {
			BlockType type = tetrisBoard_.getBlockFromBoard(row,column);
			if (BLOCK_TYPE_EMPTY != type) {
				drawSquare(column, row, type);
			}
		}
	}

	drawBlock(tetrisBoard_.currentBlock(), tetrisBoard_.getNbrOfRows() + 2);
	drawBeginArea();

	glPopMatrix();
}

void GraphicBoard::drawBeginArea() const {
	int rows = tetrisBoard_.getNbrOfRows();
	int columns = tetrisBoard_.getNbrOfColumns();

	double red = 0.8, green = 0.2, blue = 0.3;

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
	glTranslated(0,rows,0);
	glScaled(columns,2,0);
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
	for (int i = 0; i < block.nbrOfSquares(); ++i) {
		Square square = block[i];
		if (square.row_ < maxRow) {
			drawSquare(square.column_, square.row_, square.blockType_);
		}
	}
}

void GraphicBoard::drawSquare(int column, int row, BlockType blockType) {
	glColor4d(1,1,1,1);
	glPushMatrix();
	glTranslated(column,row,0);
	glTranslated(0.5,0.5,0.0);
	drawSquare(blockType);
	glPopMatrix();
}

void GraphicBoard::drawSquare(BlockType blockType) {
	mw::Sprite sprite = spriteI;
	switch (blockType) {
	case BLOCK_TYPE_J:
		sprite = spriteJ;
		break;
	case BLOCK_TYPE_L:
		sprite = spriteL;
		break;
	case BLOCK_TYPE_O:
		sprite = spriteO;
		break;
	case BLOCK_TYPE_S:
		sprite = spriteS;
		break;
	case BLOCK_TYPE_T:
		sprite = spriteT;
		break;
	case BLOCK_TYPE_Z:
		sprite = spriteZ;
		break;
	default:
		break;
	}
	sprite.draw();
}

void GraphicBoard::drawGrid() const {
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int rows = tetrisBoard_.getNbrOfRows() + 2;
	int colons = tetrisBoard_.getNbrOfColumns();

	for (int row = 0; row < rows; ++row) {
		for (int colon = 0; colon < colons; ++colon) {
			glPushMatrix();
			glTranslated(colon,row,0);
			drawGridSquare();
			glPopMatrix();
		}
	}
	glDisable(GL_BLEND);

	drawBeginArea();
}

void GraphicBoard::drawGridSquare() const {
	double red = 0.8, green = 0.2, blue = 0.3;

	// Draws the outer square
	glColor4d(red*0.8,green*0.8,blue*0.8,0.6);
	glBegin(GL_QUADS);
	glVertex2d(0.0,0.0);
	glVertex2d(1.0,0.0);
	glVertex2d(1.0,1.0);
	glVertex2d(0.0,1.0);
	glEnd();

	glPushMatrix();
	glColor4d(red*0.1,green*0.1,blue*0.1,0.1);
	glTranslated(0.1,0.1,0);
	glScaled(0.8,0.8,1.0);

	// Draws the inner square
	glBegin(GL_QUADS);
	glVertex2d(0.0,0.0);
	glVertex2d(1.0,0.0);
	glVertex2d(1.0,1.0);
	glVertex2d(0.0,1.0);
	glEnd();
	glPopMatrix();
}

void GraphicBoard::glColorWhite() const {
	glColor4d(1,1,1,1);
}

void GraphicBoard::drawFrame(double x1, double y1, double x2, double y2, double width) const {
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
