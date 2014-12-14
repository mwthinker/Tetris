#ifndef DYNAMICGRAPHICBOARD_H
#define DYNAMICGRAPHICBOARD_H

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

class DynamicGraphicBoard {
public:
	DynamicGraphicBoard();

	DynamicGraphicBoard(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void update(const PlayerPtr& player);

	void draw(float deltaTime, const BoardShader& shader);

	void updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4) {
		linesRemovedTimeLeft_ = downTime;
		removedRow1_ = row1;
		removedRow2_ = row2;
		removedRow3_ = row3;
		removedRow4_ = row4;
		//dynamicVertercies_ - 4;
	}

private:
	void setVertexAttribPointer(const BoardShader& shader);
	void updateDynamicData(const RawTetrisBoard& tetrisBoard);
	void initDynamicVbo(const RawTetrisBoard& tetrisBoard);

	void updateBoard(int &index, const RawTetrisBoard& tetrisBoard);
	void updateCurrentBlock(int &index, const RawTetrisBoard& tetrisBoard);
	void updatePreviewBlock(int &index, const RawTetrisBoard& tetrisBoard);
	
	void updateBoardLinesRemoved(float ratio);

	mw::Sprite getSprite(BlockType blockType) const;
	float squareSize_;
	float sizeBoard_;
	
	mw::VertexBufferObject vbo_;	

	float lowX_, lowY_;
	float width_, height_;
	float borderSize_;

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	std::vector<GLfloat> dynamicData_;

	int indexDynamicBoard_;
	int dynamicVertercies_;

	int rows_, columns_;

	float linesRemovedTimeLeft_;
	int removedRow1_, removedRow2_, removedRow3_, removedRow4_;
};

#endif // DYNAMICGRAPHICBOARD_H
