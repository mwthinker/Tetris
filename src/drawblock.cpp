#include "drawblock.h"
#include "color.h"

DrawBlock::DrawBlock(const BoardShader& boardShader, const TetrisEntry& spriteEntry, const Block& block, int boardHeight, float squareSize, float lowX, float lowY, bool center) : BoardVertexData(boardShader) {
	spriteZ_ = spriteEntry.getChildEntry("squareZ").getSprite();
	spriteS_ = spriteEntry.getChildEntry("squareS").getSprite();
	spriteJ_ = spriteEntry.getChildEntry("squareJ").getSprite();
	spriteI_ = spriteEntry.getChildEntry("squareI").getSprite();
	spriteL_ = spriteEntry.getChildEntry("squareL").getSprite();
	spriteT_ = spriteEntry.getChildEntry("squareT").getSprite();
	spriteO_ = spriteEntry.getChildEntry("squareO").getSprite();
	lowX_ = lowX;
	lowY_ = lowY;
	squareSize_ = squareSize;
	center_ = center;
	boardHeight_ = boardHeight;
	update(block);
}

void DrawBlock::update(const Block& block) {
	block_ = block;
	mw::Sprite sprite = getSprite(block.getBlockType());

	deltaX_ = 0.f;
	deltaY_ = 0.f;

	if (center_) {
		calculateCenterOfMass(block, deltaX_, deltaY_);
		deltaX_ = (-1.5f - deltaX_) * squareSize_;
		deltaY_ = (-1.5f - deltaY_) * squareSize_;
	}

	begin();
	for (Square sq : block) {
		Color color(1, 1, 1);
		if (sq.row_ >= boardHeight_ - 2) {
			color.alpha_ = 0;
		}
		addSquareTRIANGLES(
			lowX_ + (sq.column_ + 1)* squareSize_ + deltaX_, lowY_ + (sq.row_ + 1)* squareSize_ + deltaY_,
			squareSize_, squareSize_,
			sprite,
			color
		);
	}
	end();
}

void DrawBlock::calculateCenterOfMass(const Block& block, float& x, float& y) {
	x = 0;
	y = 0;
	for (Square sq : block) {
		x += sq.column_;
		y += sq.row_;
	}
	x = x / block.nbrOfSquares();
	y = y / block.nbrOfSquares();
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
