#ifndef DRAWBLOCK_H
#define DRAWBLOCK_H

#include "boardvertexdata.h"
#include "tetrisboard.h"
#include "boardshader.h"

#include <memory>

class DrawBlock;
using DrawBlockPtr = std::shared_ptr<DrawBlock>;

class DrawBlock : public BoardVertexData {
public:
	DrawBlock(const BoardShaderPtr& boardShader, const Block& block, int boardHeight, float squareSize, float lowX, float lowY, bool center);
	
	void update(const Block& block);

	void updateDown(const Block& block);

	void update(float deltaTime);

private:
	void calculateCenterOfMass(const Block& block, float& x, float& y);
	mw::Sprite getSprite(BlockType blockType) const;

	void updateVertexData();
	
	bool center_;
	float squareSize_;
	float lowX_, lowY_;
	int boardHeight_;
	Block block_;
	int row_;
	float movingTime_, timeLeft_;
	float deltaX_, deltaY_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWBLOCK_H
