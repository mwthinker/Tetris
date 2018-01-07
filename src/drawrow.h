#ifndef DRAWROW_H
#define DRAWROW_H

#include "player.h"
#include "boardbatch.h"

class DrawRow;
using DrawRowPtr = std::shared_ptr<DrawRow>;

class DrawRow {
public:
	DrawRow(int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);

	int getRow() const {
		return row_;
	}

	void handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void update(float deltaTime);

	bool isAlive() const;

	bool isActive() const;

	void init(int row, const TetrisBoard& board);

	const std::vector<BoardShader::Vertex>& getVertexes() {
		return vertexes_;
	}

	void clear();

private:
	void updateVertexData(const TetrisBoard& tetrisBoard);
	void updateVertexData();

	mw::Sprite getSprite(BlockType blockType) const;
		
	int columns_;
	int row_;
	int oldRow_;
	float graphicRow_;
	
	float lowX_, lowY_;
	float timeLeft_;
	int highestBoardRow_;
	float alpha_;

	const float squareSize_;
	const float fadingTime_;
	const float movingTime_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;

	std::vector<BoardShader::Vertex> vertexes_;
	std::vector<BlockType> blockTypes_;
};

#endif // DRAWROW_H
