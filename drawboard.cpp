#include "drawboard.h"
#include "gamesprite.h"

#include <mw/color.h>

DrawBoard::DrawBoard(const TetrisBoard& tetrisBoard) : tetrisBoard_(tetrisBoard) {
	height_ = 800; // height for the board

	pixlePerSquare_ = 800.0 / (tetrisBoard_.getNbrOfRows() + 2);
	color_ = mw::Color(237/256.0,78/256.0,8/256.0);
}

void DrawBoard::draw() {
	glPushMatrix();
	glTranslated(0,0,0);
	drawBoard();
	glPopMatrix();
}

double DrawBoard::getWidth() const {
	return tetrisBoard_.getNbrOfColumns() * pixlePerSquare_;
}
    
double DrawBoard::getHeight() const {
	return height_;
}

void DrawBoard::drawBoard() const {
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

void DrawBoard::drawBeginArea() const {
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

void DrawBoard::drawBlock(const Block& block) const {
	for (int i = 0; i < block.nbrOfSquares(); ++i) {
		Square square = block[i];
		drawSquare(square);
	}
}

void DrawBoard::drawSquare(const Square& square) const {
	int column = square.column;
	int row = square.row;

	glColor4d(1,1,1,1);
	glPushMatrix();
	glTranslated(column-1,row-1,0);
	glTranslated(0.5,0.5,0.0);
	drawSquare(square.blockType);
	glPopMatrix();
}

void DrawBoard::drawSquare(BlockType blockType) const {
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

void DrawBoard::drawGrid() const {
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

void DrawBoard::drawGridSquare() const {
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
