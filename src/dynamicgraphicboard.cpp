#include "dynamicgraphicboard.h"
#include "tetrisboard.h"
#include "tetrisentry.h"
#include "boardshaderfunctions.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/opengl.h>
#include <gui/component.h>

#include <sstream>
#include <vector>
#include <cassert>

namespace {

	gui::Point calculateCenter(const Block& block) {
		gui::Point point(0, 0);
		for (const Square& sq : block) {
			point.x_ += sq.column_;
			point.y_ += sq.row_;
		}

		point.x_ /= block.nbrOfSquares();
		point.y_ /= block.nbrOfSquares();

		return point;
	}

} // Anonymous namespace.

DynamicGraphicBoard::DynamicGraphicBoard() :
	INDEX_CURRENT_BLOCK(0),
	INDEX_NEXT_BLOCK(9 * 6 * 4),
	INDEX_BOARD(9 * 6 * 4 * 2),
	BLOCK_VERTICES(6 * 4 * 2) {

}

void DynamicGraphicBoard::restart(float x, float y,
	TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) {

	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares].
	dynamicData_ = std::vector<GLfloat>(sizeof(GLfloat) * 9 * 6 * (tetrisBoard.getRows() * tetrisBoard.getColumns() + 8));

	lowX_ = x;
	lowY_ = y;
	linesRemovedTime_ = 1;
	linesRemovedTimeLeft_ = -1;
	removedRow1_ = -1;
	removedRow2_ = -1;
	removedRow3_ = -1;
	removedRow4_ = -1;

	rows_ = tetrisBoard.getRows();
	columns_ = tetrisBoard.getColumns();

	auto spriteEntry = boardEntry.getChildEntry("sprites");
	spriteZ_ = spriteEntry.getChildEntry("squareZ").getSprite();
	spriteS_ = spriteEntry.getChildEntry("squareS").getSprite();
	spriteJ_ = spriteEntry.getChildEntry("squareJ").getSprite();
	spriteI_ = spriteEntry.getChildEntry("squareI").getSprite();
	spriteL_ = spriteEntry.getChildEntry("squareL").getSprite();
	spriteT_ = spriteEntry.getChildEntry("squareT").getSprite();
	spriteO_ = spriteEntry.getChildEntry("squareO").getSprite();
		
	squareSize_ = boardEntry.getChildEntry("squareSize").getFloat();
	sizeBoard_ = squareSize_ * tetrisBoard.getColumns();
	borderSize_ = boardEntry.getChildEntry("borderSize").getFloat();

	updateBlock_ = true;
	
	updateBoard(tetrisBoard);
	updateVBO();
}

void DynamicGraphicBoard::updateCurrentBlock(const Block& block) {
	updateBlock_ = true;
	currentBlock_ = block;
}

void DynamicGraphicBoard::updatePreviewBlock(BlockType type) {
	updateBlock_ = true;
	preview_ = type;
}

void DynamicGraphicBoard::updateBoard(const RawTetrisBoard& tetrisBoard) {
	int index = INDEX_BOARD;	
	dynamicVertercies_ = 0;
	// Draw the board.
	for (int row = 0; row < rows_ - 2; ++row) {
		for (int column = 0; column < columns_; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			if (type != BlockType::EMPTY && type != BlockType::WALL) {
				float x = lowX_ + borderSize_ + squareSize_ * column;
				float y = lowY_ + borderSize_ + squareSize_ * row;
				mw::Sprite sprite = getSprite(type);
				addSquareToBoardShader(dynamicData_.data(), index,
					x, y,
					squareSize_, squareSize_,
					sprite);
				dynamicVertercies_ += 6;
			}
		}
	}
}

void DynamicGraphicBoard::updateCurrentBlock() {
	int index = INDEX_CURRENT_BLOCK;
	
	// Draw the current block.
	float x = lowX_ + borderSize_;
	float y = lowY_ + borderSize_;
	mw::Sprite sprite = getSprite(currentBlock_.blockType());
	for (const Square& sq : currentBlock_) {
		if (sq.row_ < rows_ - 2) {
			addSquareToBoardShader(dynamicData_.data(), index,
				x + sq.column_ * squareSize_, y + sq.row_ * squareSize_,
				squareSize_, squareSize_,
				sprite);
		} else {
			addSquareToBoardShader(dynamicData_.data(), index,
				1000, 1000,
				squareSize_, squareSize_,
				sprite);
		}
	}
}

void DynamicGraphicBoard::updateBoardLinesRemoved() {
	int index = INDEX_BOARD;
	dynamicVertercies_ = 0;
	// Draw the board.
	for (int row = 0; row < rows_ - 2; ++row) {
		bool rowNotRemoved = !(row == removedRow1_ || row == removedRow2_
			|| row == removedRow3_ || row == removedRow4_);

		if (rowNotRemoved) {
			int linesToMoveDown = 0;
			row > removedRow1_ && removedRow1_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow2_ && removedRow2_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow3_ && removedRow3_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow4_ && removedRow4_ != -1 ? ++linesToMoveDown : 0;
			if (linesToMoveDown > 0) {
				// Swap the current row.
				std::swap_ranges(dynamicData_.data() + index,
					dynamicData_.data() + index + 9 * 6 * columns_,
					dynamicData_.data() + index + 9 * 6 * linesToMoveDown * columns_);
			}
			index += 9 * 6 * columns_;
			dynamicVertercies_ += 6 * columns_;
		}
	}
}

void DynamicGraphicBoard::updateBoardLinesRemoved(float ratio) {
	int index = INDEX_BOARD;
	// Draw the board.
	for (int row = 0; row < rows_ - 2; ++row) {
		bool rowNotRemoved = (row != removedRow1_ && row != removedRow2_
			&& row != removedRow3_ && row != removedRow4_);
		
		if (rowNotRemoved) {
			int linesToMoveDown = 0;
			row > removedRow1_ && removedRow1_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow2_ && removedRow2_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow3_ && removedRow3_ != -1 ? ++linesToMoveDown : 0;
			row > removedRow4_ && removedRow4_ != -1 ? ++linesToMoveDown : 0;
			for (int column = 0; column < columns_; ++column) {
				float dx = 0;
				float dy = -squareSize_ * linesToMoveDown;
				addSquareToBoardShader(dynamicData_.data(), index,
					dx, dy);
			}
			index += 9;
		}
	}
}

void DynamicGraphicBoard::updatePreviewBlock() {
	int index = INDEX_NEXT_BLOCK;
	
	// Draw the preview block.
	float x = lowX_ + borderSize_ + sizeBoard_ + 5 + squareSize_ * 2.5f;
	float y = lowY_ + borderSize_ + squareSize_ * (rows_ - 4) - (squareSize_ * 2.5f + 5);
	Block block = Block(preview_, 0, 0);
	gui::Point center = calculateCenter(block);
	mw::Sprite sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		addSquareToBoardShader(dynamicData_.data(), index,
			x + (-center.x_ + sq.column_ - 0.5f) * squareSize_, y + (-center.y_ + sq.row_ - 0.5f) * squareSize_,
			squareSize_, squareSize_,
			sprite);
	}
}

void DynamicGraphicBoard::updateVBO() {
	if (updateBlock_) {
		updateCurrentBlock();
		updatePreviewBlock();
		updateVBO_ = true;
		updateBlock_ = false;
	}

	if (linesRemovedTimeLeft_ > 0) {
		updateBoardLinesRemoved(1 - linesRemovedTimeLeft_ / linesRemovedTime_);
		updateVBO_ = true;
	} else {
		dynamicVertercies_ = 0;
	}

	if (vbo_.getSize() > 0) {
		if (updateVBO_) {
			vbo_.bindBuffer();
			vbo_.bindBufferSubData(0, sizeof(GLfloat) * (BLOCK_VERTICES + dynamicVertercies_) * 9, dynamicData_.data());
			vbo_.unbindBuffer();
			updateVBO_ = false;
		}
	} else {
		vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dynamicData_.size(), dynamicData_.data(), GL_DYNAMIC_DRAW);
	}
}

void DynamicGraphicBoard::draw(float deltaTime, const BoardShader& shader) {
	// Draw the dynamic part.
	if (vbo_.getSize() > 0) {
		vbo_.bindBuffer();

		if (linesRemovedTimeLeft_ > 0) {
			linesRemovedTimeLeft_ -= deltaTime;
		}
		updateVBO();

		setVertexAttribPointer(shader);
		mw::glDrawArrays(GL_TRIANGLES, 0, BLOCK_VERTICES + dynamicVertercies_);

		vbo_.unbindBuffer();
	}
}

void DynamicGraphicBoard::updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4) {
	linesRemovedTime_ = downTime;
	linesRemovedTimeLeft_ = downTime;
	removedRow1_ = row1;
	removedRow2_ = row2;
	removedRow3_ = row3;
	removedRow4_ = row4;
	// Update the board.
	updateBoardLinesRemoved();
}

mw::Sprite DynamicGraphicBoard::getSprite(BlockType blockType) const {
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
	return spriteI_;
}
