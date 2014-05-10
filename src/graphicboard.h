#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>

class GraphicPlayerInfo {
public:
	GraphicPlayerInfo();

	void update(int rowsCleared, int points, int level);

	// Draws the player info. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	// Returns the width the graphic is drawn in OpenGl.
	float getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	float getHeight() const;

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

private:
	mw::Text level_, points_, nbrOfClearedRows_;
	bool showPoints_;
};

class GraphicPreviewBlock {
public:
	GraphicPreviewBlock();

	// Draws the player info. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	void update(const BlockType& blockType, float pixlePerSquare);

	// Returns the width the graphic is drawn in OpenGl.
	float getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	float getHeight() const;

private:
	mw::Color frameColor_;
	float pixlePerSquare_;

	float width_, height_;
	Block block_;
};

// Draws a tetris board and corresponding player info.
class GraphicBoard {
public:
	GraphicBoard(int nbrOfRows, int nbrOfColumns, BlockType current, BlockType next);
		
	void update(const RawTetrisBoard& newBoard);

	// Draws the the board. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	// Returns the width the graphic is drawn in OpenGl.
	float getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	float getHeight() const;

	float getPixelPerSquare() const {
		return pixlePerSquare_;
	}

private:
	void drawBoard(const RawTetrisBoard& tetrisBoard) const;
	void drawBeginArea() const;
	
	mw::Color frameColor_;

	float pixlePerSquare_;
	float height_, width_;
	RawTetrisBoard tetrisBoard_;
};

#endif // GRAPHICBOARD_H
