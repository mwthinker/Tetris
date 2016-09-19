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
	DrawBlock(TetrisEntry spriteEntry, const Block& block, DynamicBuffer& buffer, int boardHeight, float squareSize, float lowX, float lowY, bool center);
	
	void update(const Block& block);
	void draw(const BoardShader& shader);

private:
	void calculateCenterOfMass(const Block& block, float& x, float& y);
	mw::Sprite getSprite(BlockType blockType) const;

	VertexDataPtr vd_;
	bool center_;
	float squareSize_;
	float lowX_, lowY_;
	int boardHeight_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWBLOCK_H
