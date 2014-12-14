#include "gamegraphic.h"
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
		mw::Sprite& sprite, mw::Color color = mw::Color(1,1,1)) {
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

	mw::Sprite getBoardSprite(mw::Texture texture, TetrisEntry spriteEntry) {
		float x = spriteEntry.getChildEntry("x").getFloat();
		float y = spriteEntry.getChildEntry("y").getFloat();
		float w = spriteEntry.getChildEntry("w").getFloat();
		float h = spriteEntry.getChildEntry("h").getFloat();
		return mw::Sprite(texture, x, y, w, h);
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

GameGraphic::GameGraphic() {
}

GameGraphic::GameGraphic(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares].
	dynamicData_(sizeof(GLfloat) * 9 * 6 * (tetrisBoard.getRows() * tetrisBoard.getColumns() + 8)),
	lowX_(x), lowY_(y),
	removedRow1_(-1), removedRow2_(-1), removedRow3_(-1), removedRow4_(-1) {

	rows_ = tetrisBoard.getRows();
	columns_ = tetrisBoard.getColumns();
	linesRemovedTimeLeft_ = -1;

	mw::Color color1 = boardEntry.getChildEntry("outerSquareColor").getColor();
	mw::Color color2 = boardEntry.getChildEntry("innerSquareColor").getColor();
	mw::Color color3 = boardEntry.getChildEntry("startAreaColor").getColor();
	mw::Color color4 = boardEntry.getChildEntry("playerAreaColor").getColor();
	borderColor_ = boardEntry.getChildEntry("borderColor").getColor();

	mw::Texture texture = boardEntry.getChildEntry("texture").getTexture();
	spriteZ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareZ"));
	spriteS_ = getBoardSprite(texture, boardEntry.getChildEntry("squareS"));
	spriteJ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareJ"));
	spriteI_ = getBoardSprite(texture, boardEntry.getChildEntry("squareI"));
	spriteL_ = getBoardSprite(texture, boardEntry.getChildEntry("squareL"));
	spriteT_ = getBoardSprite(texture, boardEntry.getChildEntry("squareT"));
	spriteO_ = getBoardSprite(texture, boardEntry.getChildEntry("squareO"));

	borderHorizontal_ = getBoardSprite(texture, boardEntry.getChildEntry("borderHorizontal"));
	borderVertical_ = getBoardSprite(texture, boardEntry.getChildEntry("borderVertical"));
	borderLeftUp_ = getBoardSprite(texture, boardEntry.getChildEntry("borderLeftUp"));
	borderRightUp_ = getBoardSprite(texture, boardEntry.getChildEntry("borderRightUp"));
	borderDownLeft_ = getBoardSprite(texture, boardEntry.getChildEntry("borderDownLeft"));
	borderDownRight_ = getBoardSprite(texture, boardEntry.getChildEntry("borderDownRight"));

	font_ = boardEntry.getChildEntry("font").getFont(30);
	squareSize_ = boardEntry.getChildEntry("squareSize").getFloat();
	sizeBoard_ = squareSize_ * tetrisBoard.getColumns();
	borderSize_ = boardEntry.getChildEntry("borderSize").getFloat();

	// Define all text sizes and font usage.
	level_ = mw::Text("Level ", font_, 16);
	points_ = mw::Text("Points ", font_, 16);
	nbrOfClearedRows_ = mw::Text("Rows ", font_, 16);
	showPoints_ = true;

	name_ = mw::Text("Keyboard1 ", font_, 16);

	initStaticVbo(color1, color2, color3, color4, tetrisBoard.getColumns(), tetrisBoard.getRows());
	initDynamicVbo(tetrisBoard);
}

void GameGraphic::initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows) {
	const float sizeInfo = 100;
	const float sizePlayer = lowX_ + sizeBoard_ + sizeInfo;
	int index = 0;

	width_ = squareSize_ * columns + sizeBoard_ + borderSize_ * 2;
	height_ = squareSize_ * (rows - 2) + lowY_ * 2 + +borderSize_ * 2;

	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * ((rows -2) * columns + 4) [squares]. 
	std::vector<GLfloat> data(sizeof(GLfloat) * 9 * 6 * ((rows - 2)*columns + 4)); // Hide the highest 2.
	// Draw the player area.
	float x = lowX_ + borderSize_;
	float y = lowY_ * 0.5f + borderSize_;
	addSquare(data.data() , index,
		x, y,
		sizeBoard_ + sizeInfo + 2 * lowX_, squareSize_ * (rows - 2) + lowY_,
		c4);

	// Draw the outer square.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX_ + borderSize_ + squareSize_ * column + (squareSize_ * columns + sizeInfo) * 0 + squareSize_ * 0.1f;
			y = lowY_ + borderSize_ + squareSize_ * row + squareSize_ * 0.1f;
			addSquare(data.data(), index,
				x, y,
				squareSize_ * 0.8f, squareSize_ * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX_ + borderSize_;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4);
	addSquare(data.data(), index,
		x, y,
		squareSize_ * columns, squareSize_ * 2,
		c3);

	// Draw the preview block area.
	x = lowX_ + borderSize_ + sizeBoard_ + 5;
	y = lowY_ + borderSize_ + squareSize_ * (rows - 4) - (squareSize_ * 5 + 5);
	addSquare(data.data(), index,
		x, y,
		squareSize_ * 5, squareSize_ * 5,
		c3);

	// Add border.
	// Left-up corner.
	x = lowX_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderLeftUp_,
		borderColor_);

	// Right-up corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderRightUp_,
		borderColor_);

	// Left-down corner.
	x = lowX_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownLeft_,
		borderColor_);

	// Right-down corner.
	x = lowX_ + width_ - borderSize_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, borderSize_,
		borderDownRight_,
		borderColor_);

	// Up.
	x = lowX_ + borderSize_;
	y = lowY_ + height_ - borderSize_;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Down.
	x = lowX_ + borderSize_;
	y = lowY_;
	addSquare(data.data(), index,
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal_,
		borderColor_);

	// Left.
	x = lowX_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical_,
		borderColor_);

	// Right.
	x = lowX_ + width_ - borderSize_;
	y = lowY_ + borderSize_;
	addSquare(data.data(), index,
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical_,
		borderColor_);

	staticVbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
}

void GameGraphic::initDynamicVbo(const RawTetrisBoard& tetrisBoard) {
	updateDynamicData(tetrisBoard);
	vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * dynamicData_.size(), dynamicData_.data(), GL_DYNAMIC_DRAW);
}

void GameGraphic::updateBoard(int &index, const RawTetrisBoard& tetrisBoard) {
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

void GameGraphic::updateCurrentBlock(int &index, const RawTetrisBoard& tetrisBoard) {
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

void GameGraphic::updateBoardLinesRemoved(float ratio) {
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

void GameGraphic::updatePreviewBlock(int &index, const RawTetrisBoard& tetrisBoard) {
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

void GameGraphic::updateDynamicData(const RawTetrisBoard& tetrisBoard) {
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

void GameGraphic::update(const PlayerPtr& player) {
	updateDynamicData(player->getTetrisBoard());
	PlayerInfo info = player->getPlayerInfo();
	update(info.nbrClearedRows_, info.points_, player->getLevel());
	vbo_.bindBuffer();
	mw::glBufferSubData(vbo_.getTarget(), 0, sizeof(GLfloat) * dynamicVertercies_ * 9, dynamicData_.data());
	vbo_.unbindBuffer();
}

void GameGraphic::draw(float deltaTime, const BoardShader& shader) {
	if (staticVbo_.getSize() > 0) {
		spriteI_.bindTexture(); // All sprites uses the same texture.
		staticVbo_.bindBuffer();
		setVertexAttribPointer(shader);

		// Draw the static part.
		mw::glDrawArrays(GL_TRIANGLES, 0, staticVbo_.getSize() / 9); // 9 vertices/triangle. 

		staticVbo_.unbindBuffer();

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

		mw::checkGlError();
	}
}

void GameGraphic::drawText(float x, float y, float width, float height, float scale) {
	float boardWidth = 100 * scale;
	name_.draw(x + boardWidth + borderSize_ * scale, height - y - name_.getHeight() - borderSize_ * scale);
	points_.draw(x + boardWidth + borderSize_ * scale, y + 50 * scale + borderSize_ * scale);
	level_.draw(x + boardWidth + borderSize_ * scale, y + 100 * scale + borderSize_ * scale);
	nbrOfClearedRows_.draw(x + boardWidth + borderSize_ * scale, y + 10 * scale + borderSize_ * scale);
}

void GameGraphic::update(float size, const mw::Font& font) {
	points_ = mw::Text(points_.getText(), font, size);
	name_ = mw::Text(name_.getText(), font, size);
	level_ = mw::Text(level_.getText(), font, size);
	nbrOfClearedRows_ = mw::Text(nbrOfClearedRows_.getText(), font, size);
}

mw::Sprite GameGraphic::getSprite(BlockType blockType) const {
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

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameGraphic::update(int rowsCleared, int points, int level) {
	std::stringstream stream;
	stream << "Rows " << rowsCleared;
	nbrOfClearedRows_.setText(stream.str());
	stream.str("");
	stream << "Points " << points;
	points_.setText(stream.str());
	stream.str("");
	stream << "Level " << level;
	level_.setText(stream.str());
}

void GameGraphic::setVertexAttribPointer(const BoardShader& shader) {
	shader.setGlVerA(sizeof(GLfloat) * 9, (GLvoid*) 0);
	shader.setGlTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
	shader.setGlIsTexA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));
	shader.setGlColorA(sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));
}

void GameGraphic::setName(std::string name) {
	name_.setText(name);
}
