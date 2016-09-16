#ifndef DRAWBLOCK_H
#define DRAWBLOCK_H

#include "tetrisentry.h"
#include "dynamicbuffer.h"
#include "tetrisboard.h"

#include <memory>

class DrawBlock;
using DrawBlockPtr = std::shared_ptr<DrawBlock>;

class DrawBlock {
public:
	DrawBlock(TetrisEntry spriteEntry, const Block& block, DynamicBuffer& buffer, float squareSize, float lowX, float lowY, bool center) {
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
		vd_ = buffer.pollFirstFree();
		center_ = center;
		update(block);
	}

	void update(const Block& block) {
		mw::Sprite sprite = getSprite(block.blockType());

		float deltaX = 0.f;
		float deltaY = 0.f;

		if (center_) {
			calculateCenterOfMass(block, deltaX, deltaY);
			deltaX = (-1.5f - deltaX) * squareSize_;
			deltaY = (-1.5f - deltaY) * squareSize_;
		}

		vd_->begin();
		for (Square sq : block) {
			vd_->addSquareTRIANGLES(
				lowX_ + (sq.column_ + 1)* squareSize_ + deltaX, lowY_ + (sq.row_ + 1)* squareSize_ + deltaY,
				squareSize_, squareSize_,
				sprite
			);
		}
		vd_->end();
	}

	void draw(const BoardShader& shader) {
		if (vd_) {
			vd_->drawTRIANGLES(shader);
		}
	}

private:
	void calculateCenterOfMass(const Block& block, float& x, float& y) {
		x = 0;
		y = 0;
		for (Square sq : block) {
			x += sq.column_;
			y += sq.row_;
		}		
		x = x / block.nbrOfSquares();
		y = y / block.nbrOfSquares();
	}

	mw::Sprite getSprite(BlockType blockType) const {
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

	VertexDataPtr vd_;
	bool center_;
	float squareSize_;
	float lowX_, lowY_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWBLOCK_H
