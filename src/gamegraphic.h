#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"
#include "player.h"
#include "tetrisentry.h"

#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <mw/vertexbufferobject.h>

#include <string>
#include <array>

class GameGraphic {
public:
	GameGraphic();

	GameGraphic(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void update(const PlayerPtr& player);

	inline float getWidth() const {
		return width_;
	}

	inline float getHeight() const {
		return height_;
	}

	void draw(mw::Shader& shader);

	void setMiddleMessage(const mw::Text& text);	

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

	void setName(const mw::Texture& texture);

private:
	void update(int rowsCleared, int points, int level);
	void setVertexAttribPointer(const mw::Shader& shader);
	void drawText(float x, float y, const mw::Texture& texture);
	void updateDynamicData(const RawTetrisBoard& tetrisBoard);
	void initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows);
	void initDynamicVbo(const RawTetrisBoard& tetrisBoard);

	mw::Sprite getSprite(BlockType blockType) const;

	std::vector<int> indexes_;

	float squareSize_;
	float sizeBoard_;

	mw::Texture name_;
	mw::Text middleMessage_;
	mw::Font font_;
	mw::VertexBufferObject staticVbo_, vbo_;
	int vertercies_;
	int dynamicVertercies_;

	float lowX_, lowY_;
	float width_, height_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	std::vector<GLfloat> dynamicData_;

	int indexBoard_;
	int indexCurrentBlock_;
	int indexNextBlock;

	mw::Text level_, points_, nbrOfClearedRows_;
	bool showPoints_;

	int aPosIndex_;
	int aTexIndex_;
	int aIsTexIndex_;
	int aColorIndex_;
};

#endif // GRAPHICBOARD_H
