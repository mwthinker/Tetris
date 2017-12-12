#include "drawrow.h"
#include "tetrisdata.h"

namespace {

	inline float sign(int value) {
		return value > 0 ? 1.f : -1.f;
	}

}

DrawRow::DrawRow(int row, const TetrisBoard& board, float squareSize, float lowX, float lowY) :
	fadingTime_(TetrisData::getInstance().getRowFadingTime()), squareSize_(squareSize),
	movingTime_(TetrisData::getInstance().getRowMovingTime()) {

	spriteZ_ = TetrisData::getInstance().getSprite(BlockType::Z);
	spriteS_ = TetrisData::getInstance().getSprite(BlockType::S);
	spriteJ_ = TetrisData::getInstance().getSprite(BlockType::J);
	spriteI_ = TetrisData::getInstance().getSprite(BlockType::I);
	spriteL_ = TetrisData::getInstance().getSprite(BlockType::L);
	spriteT_ = TetrisData::getInstance().getSprite(BlockType::T);
	spriteO_ = TetrisData::getInstance().getSprite(BlockType::O);
	
	lowX_ = lowX;
	lowY_ = lowY;
	init(row, board);
}

void DrawRow::init(int row, const TetrisBoard& board) {
	row_ = row;
	oldRow_ = row;
	graphicRow_ = (float) row;
	columns_ = board.getColumns();
	timeLeft_ = 0.f;
	highestBoardRow_ = board.getRows();
	alpha_ = 1.f;

	updateVertexData(board);
}

void DrawRow::handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	if (row_ >= 0) {
		int rowTobeRemoved = tetrisBoard.getRowToBeRemoved();
		switch (gameEvent) {
			case GameEvent::ROW_TO_BE_REMOVED:
				if (rowTobeRemoved < row_) {
					--row_;
					timeLeft_ += movingTime_;
				} else if (rowTobeRemoved == row_) {
					timeLeft_ = fadingTime_;
					row_ = -1;
					alpha_ = 1.f;
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
}

void DrawRow::update(float deltaTime) {
	if (oldRow_ != row_ && row_ >= 0) {
		timeLeft_ -= deltaTime;
		graphicRow_ += deltaTime / movingTime_ * sign(row_ - oldRow_);

		if (timeLeft_ < 0) {
			oldRow_ = row_;
			timeLeft_ = -0.0f;
			graphicRow_ = (float) row_;
		}
		updateVertexData();
	} else if (row_ < 0) {
		timeLeft_ -= deltaTime;
		alpha_ = timeLeft_ / fadingTime_;
		updateVertexData();
	}
}

void DrawRow::clear() {
	oldRow_ = row_;
	graphicRow_ = (float) row_;
	timeLeft_ = 0.f;
	alpha_ = 1.f;

	blockTypes_.clear();
	for (int column = 0; column < columns_; ++column) {
		blockTypes_.push_back(BlockType::EMPTY);
	}
	vertexes_.clear();
}

bool DrawRow::isAlive() const {
	return row_ >= 0;
}

bool DrawRow::isActive() const {
	return row_ >= -1 && alpha_ > 0;
}

void DrawRow::updateVertexData(const TetrisBoard& tetrisBoard) {
	blockTypes_.clear();
	for (int column = 0; column < columns_; ++column) {
		blockTypes_.push_back(tetrisBoard.getBlockType(row_, column));
	}
	updateVertexData();
}

void DrawRow::updateVertexData() {
	vertexes_.clear();
	if (row_ < highestBoardRow_ - 2) {
		for (int column = 0; column < columns_; ++column) {
			BlockType type = blockTypes_[column];
			if (type != BlockType::EMPTY) {
				addRectangle(vertexes_,
					lowX_ + column * squareSize_, lowY_ + graphicRow_ * squareSize_,
					squareSize_, squareSize_,
					getSprite(blockTypes_[column]),
					mw::Color(1, 1, 1, alpha_)
				);
			}
		}
	}
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
