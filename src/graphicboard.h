#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "rawtetrisboard.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>

// Used to draw a tetris board and corresponding player info.
class GraphicBoard {
public:
	GraphicBoard();

	void setMiddleMessage(std::string message);

	void update(int rowsCleared, int points, int level, std::string name);

	void update(const RawTetrisBoard& newBoard);

	// Draws player info and the board. Using opengl.
	void draw();

	// Returns the width the graphic requires to draw (draw()) everything.
	double getWidth() const;

	// Returns the height the graphic requires to draw (draw()) everything.
	double getHeight() const;

private:
	void drawBorder() const;
	void drawInfo(const RawTetrisBoard& tetrisBoard);
	void drawPreviewBlock(const Block& block);
	void drawBoard(const RawTetrisBoard& tetrisBoard);
	void drawBeginArea() const;
	void drawBlock(const Block& block, int maxRow);

	mw::Text name_, level_, points_, nbrOfClearedRows_;
	mw::Text middleMessage_;

	double pixlePerSquare_;
	mw::Color frameColor_;
	int columns_, rows_;

	double height_; // Height for the board
	double voidHeight_; // Height between board and outside screen, up/down and to the left.
	double borderLineThickness_; // Line thickness for the border.
	double previwBorderSizeInSquares_; // The preview border size in number of squares.
	double horizontalDistanceToText_; // Text horizontal distance between board and text.

	RawTetrisBoard tetrisBoard_;
};

#endif // GRAPHICBOARD_H
