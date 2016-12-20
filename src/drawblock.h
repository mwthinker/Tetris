#ifndef DRAWBLOCK_H
#define DRAWBLOCK_H

#include "tetrisentry.h"
#include "boardvertexdata.h"
#include "tetrisboard.h"
#include "boardshader.h"

#include <memory>

class DrawBlock;
using DrawBlockPtr = std::shared_ptr<DrawBlock>;

class DrawBlock : public BoardVertexData {
public:
	DrawBlock(const BoardShader& boardShader, const TetrisEntry& spriteEntry, const Block& block, int boardHeight, float squareSize, float lowX, float lowY, bool center);
	
	void update(const Block& block);

private:
	void calculateCenterOfMass(const Block& block, float& x, float& y);
	mw::Sprite getSprite(BlockType blockType) const;

	bool center_;
	float squareSize_;
	float lowX_, lowY_;
	int boardHeight_;
	Block block_;
	float deltaX_, deltaY_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWBLOCK_H
