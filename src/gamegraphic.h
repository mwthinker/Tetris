#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"
#include "player.h"
#include "tetrisentry.h"

#include <mw/text.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/vertexbufferobject.h>

#include <gui/windowmatrix.h>

#include <string>

class GameGraphic {
public:
	GameGraphic();

	GameGraphic(TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void update(const PlayerPtr& player);

	inline float getWidth() const {
		return width_;
	}

	inline float getHeight() const {
		return height_;
	}

	void draw(mw::Shader& shader);

	void setMiddleMessage(const mw::Text& text);

private:
	void updateDynamicData(const RawTetrisBoard& tetrisBoard);
	void initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows);
	void initDynamicVbo(const RawTetrisBoard& tetrisBoard);

	mw::Sprite getSprite(BlockType blockType) const;

	std::vector<int> indexes_;

	float squareSize_;
	float sizeBoard_;

	mw::Text name_;
	mw::Text middleMessage_;
	mw::Font font_;
	mw::VertexBufferObject staticVbo_, vbo_;
	int vertercies_;
	int dynamicVertercies_;

	float width_, height_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	std::vector<GLfloat> dynamicData_;
	//gui::Point 

	int indexBoard_;
	int indexCurrentBlock_;
	int indexNextBlock;
};

class GraphicPlayerInfo {
public:
	GraphicPlayerInfo(const mw::Font& font);

	void update(int rowsCleared, int points, int level);

	// Draws the player info. Using opengl.
	// The game is drawn in (x,y) = ([0, getWidth()], [0, getHeight()]).
	void draw(gui::WindowMatrixPtr wp);

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

#endif // GRAPHICBOARD_H
