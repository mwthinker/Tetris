#include "drawrow.h"
#include "tetrisdata.h"

namespace {

	inline float sign(int value) {
		return value > 0 ? 1.f : -1.f;
	}

}

DrawRow::DrawRow(const BoardShaderPtr& boardShader, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY) : BoardVertexData(boardShader) {
	spriteZ_ = TetrisData::getInstance().getSprite(BlockType::Z);
	spriteS_ = TetrisData::getInstance().getSprite(BlockType::S);
	spriteJ_ = TetrisData::getInstance().getSprite(BlockType::J);
	spriteI_ = TetrisData::getInstance().getSprite(BlockType::I);
	spriteL_ = TetrisData::getInstance().getSprite(BlockType::L);
	spriteT_ = TetrisData::getInstance().getSprite(BlockType::T);
	spriteO_ = TetrisData::getInstance().getSprite(BlockType::O);
	lowX_ = lowX;
	lowY_ = lowY;
	squareSize_ = squareSize;
	init(row, board);
}

void DrawRow::init(int row, const TetrisBoard& board) {
	row_ = row;
	oldRow_ = row;
	graphicRow_ = (float) row;
	columns_ = board.getColumns();
	timeLeft_ = 0.f;
	movingTime_ = 0.05f;

	updateVertexData(board);
}

void DrawRow::handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	int rowTobeRemoved = tetrisBoard.getRowToBeRemoved();
	switch (gameEvent) {
		case GameEvent::ROW_TO_BE_REMOVED:
			if (rowTobeRemoved < row_) {
				--row_;
				timeLeft_ += movingTime_;
			} else if (rowTobeRemoved == row_) {
				row_ = -1;
			}
			break;
		case GameEvent::FOUR_ROW_REMOVED:
		case GameEvent::THREE_ROW_REMOVED:
		case GameEvent::TWO_ROW_REMOVED:
		case GameEvent::ONE_ROW_REMOVED:
			break;
		case GameEvent::EXTERNAL_ROWS_ADDED:
			row_ += tetrisBoard.getNbrExternalRowsAdded();
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
		graphicRow_ += deltaTime / movingTime_ * sign(row_ - oldRow_);

		if (timeLeft_ < 0) {
			oldRow_ = row_;
			timeLeft_ = -0.0f;
			graphicRow_ = (float) row_;
		}
		updateVertexData();
	}

	if (isAlive()) {
		BoardVertexData::drawTRIANGLES();
	}
}

bool DrawRow::isAlive() const {
	return row_ >= 0;
}


void DrawRow::updateVertexData(const TetrisBoard& tetrisBoard) {
	begin();
	for (int column = 0; column < columns_; ++column) {
		BlockType type = tetrisBoard.getBlockType(row_, column);

		mw::Color color(1, 1, 1, 1);
		if (type == BlockType::EMPTY) {
			color = mw::Color(1, 1, 1, 0);
		}
		addSquareTRIANGLES(
			lowX_ + column * squareSize_, lowY_ + graphicRow_ * squareSize_,
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
			lowX_ + column * squareSize_, lowY_ + graphicRow_ * squareSize_,
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
