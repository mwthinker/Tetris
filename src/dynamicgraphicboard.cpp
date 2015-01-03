#include "dynamicgraphicboard.h"
#include "tetrisboard.h"
#include "tetrisentry.h"

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

	inline void addVertex(GLfloat* data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		mw::Color color) {

		data[index++] = x;
		data[index++] = y;

		data[index++] = xTex;
		data[index++] = yTex;

		data[index++] = isTex;

		data[index++] = color.red_;
		data[index++] = color.green_;
		data[index++] = color.blue_;
		data[index++] = color.alpha_;
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(GLfloat* data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		mw::Color color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Color color) {

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
	}

	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Sprite& sprite, mw::Color color = mw::Color(1, 1, 1)) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			sprite.getX() / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
	}

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

DynamicGraphicBoard::DynamicGraphicBoard() {
}

DynamicGraphicBoard::DynamicGraphicBoard(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares].
	dynamicData_(sizeof(GLfloat) * 9 * 6 * (tetrisBoard.getRows() * tetrisBoard.getColumns() + 8)),
	lowX_(x), lowY_(y),
	removedRow1_(-1), removedRow2_(-1), removedRow3_(-1), removedRow4_(-1) {

	rows_ = tetrisBoard.getRows();
	columns_ = tetrisBoard.getColumns();
	linesRemovedTimeLeft_ = 0;

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
	initDynamicVbo(tetrisBoard);
}

void DynamicGraphicBoard::initDynamicVbo(const RawTetrisBoard& tetrisBoard) {
	updateDynamicData(tetrisBoard);
	vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dynamicData_.size(), dynamicData_.data(), GL_DYNAMIC_DRAW);
}

void DynamicGraphicBoard::updateBoard(int &index, const RawTetrisBoard& tetrisBoard) {
	// Draw the board.
	for (int row = 0; row < rows_ - 2; ++row) {
		for (int column = 0; column < columns_; ++column) {
			BlockType type = tetrisBoard.getBlockType(row, column);
			if (type != BlockType::EMPTY && type != BlockType::WALL) {
				float x = lowX_ + borderSize_ + squareSize_ * column;
				float y = lowY_ + borderSize_ + squareSize_ * row;
				mw::Sprite sprite = getSprite(type);
				addSquare(dynamicData_.data(), index,
					x, y,
					squareSize_, squareSize_,
					sprite);
				dynamicVertercies_ += 6;
			}
		}
	}
}

void DynamicGraphicBoard::updateCurrentBlock(int &index, const RawTetrisBoard& tetrisBoard) {
	// Draw the current block.
	float x = lowX_ + borderSize_;
	float y = lowY_ + borderSize_;
	Block block = tetrisBoard.getBlock();
	mw::Sprite sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		if (sq.row_ < rows_ - 2) {
			addSquare(dynamicData_.data(), index,
				x + sq.column_ * squareSize_, y + sq.row_ * squareSize_,
				squareSize_, squareSize_,
				sprite);
			dynamicVertercies_ += 6;
		}
	}
}

void DynamicGraphicBoard::updateBoardLinesRemoved(float ratio) {
	int index = indexDynamicBoard_;
	// Draw the board.
	for (int row = 0; row < rows_ - 2; ++row) {
		if (row != removedRow1_ || row != removedRow2_ || row != removedRow3_ || row != removedRow4_) {
			for (int column = 0; column < columns_; ++column) {
				BlockType type;// = tetrisBoard.getBlockType(row, column);
				if (type != BlockType::EMPTY && type != BlockType::WALL) {
					float x = lowX_ + borderSize_ + squareSize_ * column;
					float y = lowY_ + borderSize_ + squareSize_ * row;
					mw::Sprite sprite = getSprite(type);
					addSquare(dynamicData_.data(), index,
						x, y,
						squareSize_, squareSize_,
						sprite);
					dynamicVertercies_ += 6;
				}
			}
		}
	}
}

void DynamicGraphicBoard::updatePreviewBlock(int &index, const RawTetrisBoard& tetrisBoard) {
	// Draw the preview block.
	float x = lowX_ + borderSize_ + sizeBoard_ + 5 + squareSize_ * 2.5f;
	float y = lowY_ + borderSize_ + squareSize_ * (rows_ - 4) - (squareSize_ * 2.5f + 5);
	Block block = Block(tetrisBoard.getNextBlockType(), 0, 0);
	gui::Point center = calculateCenter(block);
	mw::Sprite sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		addSquare(dynamicData_.data(), index,
			x + (-center.x_ + sq.column_ - 0.5f) * squareSize_, y + (-center.y_ + sq.row_ - 0.5f) * squareSize_,
			squareSize_, squareSize_,
			sprite);
		dynamicVertercies_ += 6;
	}
}

void DynamicGraphicBoard::updateDynamicData(const RawTetrisBoard& tetrisBoard) {
	dynamicVertercies_ = 0; // Reset Number of vertices.
	int index = 0;

	updateCurrentBlock(index, tetrisBoard);

	// Draw the preview block.
	updatePreviewBlock(index, tetrisBoard);

	indexDynamicBoard_ = index;
	if (linesRemovedTimeLeft_ < 0) {
		// Update the board.
		updateBoard(index, tetrisBoard);
	} else {
		// Update the board.
		updateBoardLinesRemoved(0);
	}
}

void DynamicGraphicBoard::update(const PlayerPtr& player) {
	updateDynamicData(player->getTetrisBoard());
	vbo_.bindBuffer();
	mw::glBufferSubData(vbo_.getTarget(), 0, sizeof(GLfloat) * dynamicVertercies_ * 9, dynamicData_.data());
	vbo_.unbindBuffer();
}

void DynamicGraphicBoard::draw(float deltaTime, const BoardShader& shader) {
	// Draw the dynamic part.
	if (vbo_.getSize() > 0) {
		vbo_.bindBuffer();

		if (linesRemovedTimeLeft_ > 0) {
			linesRemovedTimeLeft_ -= deltaTime;
			updateBoardLinesRemoved(deltaTime);
		}
		setVertexAttribPointer(shader);
		mw::glDrawArrays(GL_TRIANGLES, 0, dynamicVertercies_);

		vbo_.unbindBuffer();
	}
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
		default:
			assert(0); // Should not be here.
			return mw::Sprite();
	}
}

void DynamicGraphicBoard::setVertexAttribPointer(const BoardShader& shader) {
	shader.setGlVerA(sizeof(GLfloat) * 9, (GLvoid*) 0);
	shader.setGlTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
	shader.setGlIsTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));
	shader.setGlColorA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
}
