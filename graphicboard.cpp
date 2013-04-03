#include "graphicboard.h"

#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisboard.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>

#include <SDL_opengl.h>
#include <sstream>
#include "color.h"

GraphicBoard::GraphicBoard(const TetrisBoard& tetrisBoard) : tetrisBoard_(tetrisBoard) {	
	height_ = 800; // height for the board
	voidHeight_ = 10;
	borderLineThickness_ = 7; // Should be lower than voidHeight_
	previwBorderSizeInSquares_ = 5;
	horizontalDistanceToText_ = 40;

	pixlePerSquare_ = 800.0 / (tetrisBoard_.getNbrOfRows() + 2);
	color_ = Color(237/256.0,78/256.0,8/256.0);

	text1_ = mw::Text("Player",fontDefault);
	text2_ = mw::Text("Level ",fontDefault);
	text3_ = mw::Text("Points",fontDefault);
	text4_ = mw::Text("Rows",fontDefault);

	gameOverMessage_ = mw::Text("",fontDefault);
	gameOverMessage_.setCharacterSize(70);

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

void GraphicBoard::draw() {
	glPushMatrix();
	glTranslated(voidHeight_,voidHeight_,0.0);
	drawBoard();
	drawInfo();
	glPopMatrix();	

	if (tetrisBoard_.isGameOver()) {
		glPushMatrix();
		// Put it in the middle.
		glTranslated(getWidth()*0.5-gameOverMessage_.getWidth()*0.5,getHeight()*0.5-gameOverMessage_.getHeight()*0.5,0.0);
		glColor3d(1,1,1);
		gameOverMessage_.draw();
		glPopMatrix();
	}
	
	drawBorder();
}

double GraphicBoard::getWidth() const {
	if (previwBorderSizeInSquares_*pixlePerSquare_ > text1_.getWidth()) {
		return tetrisBoard_.getNbrOfColumns()*pixlePerSquare_ + horizontalDistanceToText_ + previwBorderSizeInSquares_*pixlePerSquare_ + voidHeight_*2;
	} else {
		std::cout << text1_.getWidth() << "\n";
		return tetrisBoard_.getNbrOfColumns()*pixlePerSquare_ + horizontalDistanceToText_ + text1_.getWidth() + voidHeight_*2;
	}
}

double GraphicBoard::getHeight() const {
	return height_ + voidHeight_*2;
}

// Private
void GraphicBoard::drawBorder() {
	glPushMatrix();
	color_.glColor();
	drawFrame(0,0,getWidth(),getHeight(), borderLineThickness_);
	//drawFrame(100,100,200,200, 7);
	glPopMatrix();
}

void GraphicBoard::drawInfo() {
	glPushMatrix();
	//glScaled(pixlePerSquare_,pixlePerSquare_,1.0);
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
	ss << "Level " << level_;//tetrisBoard_->getLevel();
	text2_.setText(ss.str());
	glColorWhite();
	text2_.draw();
	glTranslated(0,-40,0);

	ss.str("");
	ss << "Points " << points_;//tetrisBoard_->getPoints();
	text3_.setText(ss.str());
	text3_.draw();
	glTranslated(0,-40,0);


	ss.str("");
	ss << "Rows " << nbrOfClearedRows_;//tetrisBoard_->nbrOfClearedRows();
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
		x += square.column*1.0/nbrOfSquares;
		y += square.row*1.0/nbrOfSquares;
	}

	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);
	glTranslated(-x+0.5,-y+0.5,0); // Why +0.5?
	drawBlock(block);
	glPopMatrix();
	glPushMatrix();
	// Size is 5 squares.
	glScaled(previwBorderSizeInSquares_*pixlePerSquare_,previwBorderSizeInSquares_*pixlePerSquare_,1);
	color_.glColor();
	// Makes the line thickness to borderLineThickness_.
	drawFrame(-0.5,-0.5,0.5,0.5,1.0/(previwBorderSizeInSquares_*pixlePerSquare_)*borderLineThickness_);
	glPopMatrix();
}

void GraphicBoard::drawBoard() {
	glPushMatrix();
	glScaled(pixlePerSquare_,pixlePerSquare_,1.0);

	drawGrid();

	const Squares& squares = tetrisBoard_.getGameBoard();
	for (Squares::const_iterator it = squares.begin(); it != squares.end(); ++it) {
		const Square& square = *it;
		drawSquare(square);
	}

	drawBlock(tetrisBoard_.currentBlock());
	drawBeginArea();

	glPopMatrix();
}

void GraphicBoard::drawBeginArea() {
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

void GraphicBoard::drawBlock(const Block& block) {
	for (int i = 0; i < block.nbrOfSquares(); ++i) {
		Square square = block[i];
		drawSquare(square);
	}
}

void GraphicBoard::drawSquare(const Square& square) {
	int column = square.column;
	int row = square.row;

	glColor4d(1,1,1,1);
	glPushMatrix();
	glTranslated(column-1,row-1,0);
	glTranslated(0.5,0.5,0.0);
	drawSquare(square.blockType);
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

void GraphicBoard::drawGrid() {
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glColor4d(1,0.5,0.5,0.5);

	int rows = tetrisBoard_.getNbrOfRows();
	int colons = tetrisBoard_.getNbrOfColumns();

	//std::cout << "\n" << rows << " " << colons;

	for (int row = 0; row < rows; ++row) {
		for (int colon = 0; colon < colons; ++colon) {
			glPushMatrix();
			glTranslated(colon,row,0);
			//glScaled(0.5,0.5,0.5);

			drawGridSquare();
			glPopMatrix();
		}
	}
	glDisable(GL_BLEND);

	drawBeginArea();
}

void GraphicBoard::drawGridSquare() {
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

void GraphicBoard::drawFrame(double x1, double y1, double x2, double y2, double width) {
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
