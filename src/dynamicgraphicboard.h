#ifndef DYNAMICGRAPHICBOARD_H
#define DYNAMICGRAPHICBOARD_H

#include "tetrisentry.h"
#include "boardshader.h"
#include "block.h"

#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <mw/vertexbufferobject.h>

#include <string>
#include <vector>

class RawTetrisBoard;

class DynamicGraphicBoard {
public:
	DynamicGraphicBoard();

	void restart(float x, float y,
		TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void draw(float deltaTime, const BoardShader& shader);

	void updateBoard(const RawTetrisBoard& tetrisBoard);

	void updateCurrentBlock(const Block& block);

	void updatePreviewBlock(BlockType type);

	void updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4);

private:
	void updateVBO();
	void updateCurrentBlock();
	void updatePreviewBlock();
	
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

	int dynamicVertercies_;

	int rows_, columns_;

	float linesRemovedTimeLeft_;
	float linesRemovedTime_;
	int removedRow1_, removedRow2_, removedRow3_, removedRow4_;
	
	bool updateBlock_;
	bool updateVBO_;
	Block currentBlock_;
	BlockType preview_;

	const int INDEX_CURRENT_BLOCK;
	const int INDEX_NEXT_BLOCK;
	const int INDEX_BOARD;
	const int BLOCK_VERTICES;
};

#endif // DYNAMICGRAPHICBOARD_H
