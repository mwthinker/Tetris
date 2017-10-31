#ifndef DRAWBLOCK_H
#define DRAWBLOCK_H

#include "tetrisboard.h"
#include "boardshader.h"
#include "boardbatch.h"

#include <vector>

class DrawBlock {
public:
	DrawBlock() = default;

	DrawBlock(const Block& block, int boardHeight, float squareSize, float lowX, float lowY, bool center);
	
	void update(const Block& block);

	void updateDown(const Block& block);

	void update(float deltaTime);

	const std::vector<BoardShader::Vertex>& getVertexes() {
		return vertexes_;
	}

private:
	void calculateCenterOfMass(const Block& block, float& x, float& y);
	mw::Sprite getSprite(BlockType blockType) const;

	void updateVertexData();
	
	std::vector<BoardShader::Vertex> vertexes_;
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
