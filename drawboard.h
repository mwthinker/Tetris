#ifndef DRAWBOARD_H
#define DRAWBOARD_H

#include "color.h"
#include "tetrisboard.h"

#include <mw/text.h>

#include <string>

class DrawBoard {
public:
	DrawBoard(const TetrisBoard& tetrisBoard);

	// Draws player info and the board. Using opengl.
    void draw();

	// Returns the width the graphic requires to draw (draw()) everything.
    double getWidth() const;
    
	// Returns the height the graphic requires to draw (draw()) everything.
	double getHeight() const;

private:
	void drawBeginArea() const;
	void drawBoard() const;
	void drawGrid() const;
	void drawBlock(const Block& block) const;
    void drawSquare(const Square& square) const;
    void drawSquare(BlockType blockType) const;
	void drawGridSquare() const;

	double height_; // Height for the board
	double pixlePerSquare_;
	const TetrisBoard& tetrisBoard_;
	Color color_;
};

#endif // DRAWBOARD_H