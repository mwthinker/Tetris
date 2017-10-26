#include "drawblock.h"
#include "tetrisdata.h"

DrawBlock::DrawBlock(const BoardShaderPtr& boardShader, const Block& block, int boardHeight, float squareSize, float lowX, float lowY, bool center) : BoardVertexData(boardShader) {
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
	center_ = center;
	boardHeight_ = boardHeight;
	timeLeft_ = -0.001f;
	movingTime_ = 1.5f;
	update(block);
}

void DrawBlock::update(const Block& block) {
	row_ = block.getLowestRow();
	timeLeft_ = -1;
	block_ = block;
	mw::Sprite sprite = getSprite(block.getBlockType());

	deltaX_ = 0.f;
	deltaY_ = 0.f;

	if (center_) {
		calculateCenterOfMass(block, deltaX_, deltaY_);
		deltaX_ = (-0.5f - deltaX_) * squareSize_;
		deltaY_ = (-0.5f - deltaY_) * squareSize_;
	}

	begin();
	for (Square sq : block) {
		mw::Color color(1, 1, 1);
		if (sq.row_ >= boardHeight_ - 2) {
			color.alpha_ = 0;
		}
		addSquareTRIANGLES(
			lowX_ + sq.column_ * squareSize_ + deltaX_, lowY_ + sq.row_ * squareSize_ + deltaY_,
			squareSize_, squareSize_,
			sprite,
			color
		);
	}
	end();
}

void DrawBlock::updateDown(const Block& block) {
	row_ = block_.getLowestRow();
	block_ = block;
	timeLeft_ = movingTime_;
}

void DrawBlock::update(float deltaTime) {
	if (timeLeft_ >= 0) {
		timeLeft_ += -deltaTime;

		if (timeLeft_ < 0) {
			row_ = block_.getLowestRow();
			timeLeft_ = -0.1f;
		}
		updateVertexData();
	}
}

void DrawBlock::updateVertexData() {
	begin();
	for (Square sq : block_) {
		mw::Color color(1, 1, 1);
		if (sq.row_ >= boardHeight_ - 2) {
			color.alpha_ = 0;
		}
		
		updateSquareTRIANGLES(
			lowX_ + sq.column_ * squareSize_, lowY_ + (row_ - sq.row_) * timeLeft_ / movingTime_ * squareSize_  + sq.row_ *  squareSize_,
			squareSize_, squareSize_
		);
	}
	end();
}

void handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	switch (gameEvent) {
		case GameEvent::ROW_TO_BE_REMOVED:
			break;
		case GameEvent::FOUR_ROW_REMOVED:
		case GameEvent::THREE_ROW_REMOVED:
		case GameEvent::TWO_ROW_REMOVED:
		case GameEvent::ONE_ROW_REMOVED:
			break;
		case GameEvent::BLOCK_COLLISION:
			break;
	}
}

void DrawBlock::calculateCenterOfMass(const Block& block, float& x, float& y) {	
	x = 0;
	y = 0;
	for (const Square& sq : block) {
		x += sq.column_;
		y += sq.row_;
	}
	x = x / block.getSize();
	y = y / block.getSize();
}

mw::Sprite DrawBlock::getSprite(BlockType blockType) const {
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
