#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisboard.h"
#include "player.h"
#include "tetrisentry.h"
#include "boardshader.h"

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

	void draw(float deltaTime, const BoardShader& shader);
	void drawText(float x, float y, float width, float height, float scale);

	void update(float size, const mw::Font& font);

	void setMiddleMessage(const mw::Text& text);

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

	void setName(std::string name);

	void updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4) {
		linesRemovedTimeLeft_ = downTime;
		removedRow1_ = row1;
		removedRow2_ = row2;
		removedRow3_ = row3;
		removedRow4_ = row4;
		//dynamicVertercies_ - 4;
	}

private:
	void update(int rowsCleared, int points, int level);
	void setVertexAttribPointer(const BoardShader& shader);
	void updateDynamicData(const RawTetrisBoard& tetrisBoard);
	void initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows);
	void initDynamicVbo(const RawTetrisBoard& tetrisBoard);

	void updateBoard(int &index, const RawTetrisBoard& tetrisBoard);
	void updateCurrentBlock(int &index, const RawTetrisBoard& tetrisBoard);
	void updatePreviewBlock(int &index, const RawTetrisBoard& tetrisBoard);
	
	void updateBoardLinesRemoved(float ratio);

	mw::Sprite getSprite(BlockType blockType) const;
	float squareSize_;
	float sizeBoard_;

	mw::Text middleMessage_;
	mw::Font font_;
	mw::VertexBufferObject staticVbo_, vbo_;	

	float lowX_, lowY_;
	float width_, height_;
	float borderSize_;
	mw::Color borderColor_;

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	mw::Sprite borderHorizontal_, borderVertical_, borderLeftUp_, borderRightUp_, borderDownLeft_, borderDownRight_;
	std::vector<GLfloat> dynamicData_;

	int indexDynamicBoard_;
	int dynamicVertercies_;

	mw::Text level_, points_, nbrOfClearedRows_, name_;
	bool showPoints_;

	int rows_, columns_;

	float linesRemovedTimeLeft_;
	int removedRow1_, removedRow2_, removedRow3_, removedRow4_;
};

#endif // GRAPHICBOARD_H
