#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"

#include <mw/text.h>
#include <mw/color.h>

#include <string>

class GraphicPlayerInfo {
public:
	GraphicPlayerInfo(const mw::Font& font);

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
	mw::Font font_;
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

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_, empty_;

private:
	const mw::Sprite& getSprite(BlockType blockType) const {
		switch (blockType) {
			case BlockType::I:
				return spriteI_;
			case BlockType::J:
				return spriteJ_;
			case BlockType::L:
				return spriteL_;
			case BlockType::O:
				return spriteO_;
			case BlockType::S:
				return spriteS_;
			case BlockType::T:
				return spriteT_;
			case BlockType::Z:
				return spriteZ_;
		}
		return empty_;
	}

	mw::Color frameColor_;
	float pixlePerSquare_;

	float width_, height_;
	Block block_;
};

// Draws a tetris board and corresponding player info.
class GraphicBoard {
public:
	GraphicBoard();	

	GraphicBoard(const TetrisBoard& tetrisBoard);
		
	void update();

	// Draws the the board. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw();

	// Returns the width the graphic is drawn in OpenGl.
	float getWidth() const;

	// Returns the height the graphic is drawn in OpenGl.
	float getHeight() const;

	inline float getPixelPerSquare() const {
		return pixlePerSquare_;
	}

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_, empty_;

private:
	void drawBoard(const RawTetrisBoard& tetrisBoard) const;
	void drawBeginArea() const;

	const mw::Sprite& getSprite(BlockType blockType) const {
		switch (blockType) {
			case BlockType::I:
				return spriteI_;
			case BlockType::J:
				return spriteJ_;
			case BlockType::L:
				return spriteL_;
			case BlockType::O:
				return spriteO_;
			case BlockType::S:
				return spriteS_;
			case BlockType::T:
				return spriteT_;
			case BlockType::Z:
				return spriteZ_;
		}
		return empty_;
	}
	
	mw::Color frameColor_;

	float pixlePerSquare_;
	float height_, width_;
	
	const TetrisBoard* tetrisBoard_;
};

#endif // GRAPHICBOARD_H
