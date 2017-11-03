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

private:
	void updateVertexData(const TetrisBoard& tetrisBoard);
	void updateVertexData();

	mw::Sprite getSprite(BlockType blockType) const;

	std::vector<BoardShader::Vertex> vertexes_;
	std::vector<BlockType> blockTypes_;
	int columns_;
	int row_;
	int oldRow_;
	float graphicRow_;
	float squareSize_;
	float lowX_, lowY_;
	float timeLeft_;
	int highestBoardRow_;
	float alpha_;

	float movingTime_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWROW_H
