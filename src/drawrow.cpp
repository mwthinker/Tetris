#include "drawrow.h"
#include "color.h"

namespace {

	inline float sign(int value) {
		return value > 0 ? 1.f : -1.f;
	}

}

DrawRow::DrawRow(TetrisEntry spriteEntry, const BoardShader& boardShader, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY) : BoardVertexData(boardShader) {
	spriteZ_ = spriteEntry.getChildEntry("squareZ").getSprite();
	spriteS_ = spriteEntry.getChildEntry("squareS").getSprite();
	spriteJ_ = spriteEntry.getChildEntry("squareJ").getSprite();
	spriteI_ = spriteEntry.getChildEntry("squareI").getSprite();
	spriteL_ = spriteEntry.getChildEntry("squareL").getSprite();
	spriteT_ = spriteEntry.getChildEntry("squareT").getSprite();
	spriteO_ = spriteEntry.getChildEntry("squareO").getSprite();
	init(row, board, squareSize, lowX, lowY);
}

void DrawRow::init(int row, const TetrisBoard& board, float squareSize, float lowX, float lowY) {
	lowX_ = lowX;
	lowY_ = lowY;
	row_ = row;
	oldRow_ = row;
	graphicRow_ = (float) row;
	squareSize_ = squareSize;
	columns_ = board.getColumns();
	timeLeft_ = 0.f;
	movingTime_ = 0.05f;

	updateVertexData(board);
}

void DrawRow::handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	int rowTobeRemoved = tetrisBoard.getRowToBeRemoved();
	switch (gameEvent) {
		case GameEvent::ROW_TO_BE_REMOVED:
			if (tetrisBoard.getRowToBeRemoved() < row_) {
				--row_;
				timeLeft_ += movingTime_;
			} else if (tetrisBoard.getRowToBeRemoved() == row_) {
				row_ = -1;
			}
			break;
		case GameEvent::FOUR_ROW_REMOVED:
		case GameEvent::THREE_ROW_REMOVED:
		case GameEvent::TWO_ROW_REMOVED:
		case GameEvent::ONE_ROW_REMOVED:
			break;
		case GameEvent::BLOCK_COLLISION:
			if (row_ >= 0) {
				updateVertexData(tetrisBoard);
			}
			break;
	}
}

void DrawRow::draw(float deltaTime) {	
	if (oldRow_ != row_ && row_ >= 0) {
		timeLeft_ += -deltaTime;
		graphicRow_ += deltaTime * 1 / movingTime_ * sign(row_ - oldRow_);

		if (timeLeft_ < 0) {
			oldRow_ = row_;
			timeLeft_ = -0.0f;
			graphicRow_ = (float) row_;
		}
		updateVertexData();
	}
	

	if (isAlive()) {
		BoardVertexData::draw(GL_TRIANGLES);
	}
}

bool DrawRow::isAlive() const {
	return row_ >= 0;
}


void DrawRow::updateVertexData(const TetrisBoard& tetrisBoard) {
	begin();
	for (int column = 0; column < columns_; ++column) {
		BlockType type = tetrisBoard.getBlockType(row_, column);

		Color color(1, 1, 1, 1);
		if (type == BlockType::EMPTY) {
			color = Color(1, 1, 1, 0);
		}
		addSquareTRIANGLES(
			lowX_ + (column + 1) * squareSize_, lowY_ + (graphicRow_ + 1)* squareSize_,
			squareSize_, squareSize_,
			getSprite(type),
			color
		);
	}
	end();
}

void DrawRow::updateVertexData() {
	begin();
	for (int column = 0; column < columns_; ++column) {
		updateSquareTRIANGLES(
			(column + 1) * squareSize_, (graphicRow_ + 1) * squareSize_,
			squareSize_, squareSize_
		);
	}
	end();
}

mw::Sprite DrawRow::getSprite(BlockType blockType) const {
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
