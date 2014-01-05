#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "rawtetrisboard.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>

class GraphicPlayerInfo {
public:
	GraphicPlayerInfo();

	void update(int rowsCleared, int points, int level, std::string name);

	// Draws the player info. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	// Returns the width the graphic is drawn in OpenGl.
	double getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	double getHeight() const;

private:
	mw::Text name_, level_, points_, nbrOfClearedRows_;
	
	const double WIDTH = 200;
	const double HEIGHT = 200;
};

class GraphicPreviewBlock {
public:
	GraphicPreviewBlock();

	// Draws the player info. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	void update(const Block& block);

	// Returns the width the graphic is drawn in OpenGl.
	double getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	double getHeight() const;

private:
	double previwBorderSizeInSquares_; // The preview border size in number of squares.
	double borderLineThickness_; // Line thickness for the border.
	mw::Color frameColor_;
	double pixlePerSquare_;

	double width_, height_;
	Block block_;
};

// Draws a tetris board and corresponding player info.
class GraphicBoard {
public:
	GraphicBoard();
		
	void update(const RawTetrisBoard& newBoard);

	// Draws the the board. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	// Returns the width the graphic is drawn in OpenGl.
	double getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	double getHeight() const;

private:
	void drawBoard(const RawTetrisBoard& tetrisBoard) const;
	void drawBeginArea() const;
	
	mw::Color frameColor_;

	double pixlePerSquare_;
	double height_, width_;
	
	RawTetrisBoard tetrisBoard_;
};

#endif // GRAPHICBOARD_H
