#ifndef DRAWROW_H
#define DRAWROW_H

#include "dynamicbuffer.h"
#include "tetrisentry.h"
#include "player.h"

#include <mw/vertexbufferobject.h>

class DrawRow;
using DrawRowPtr = std::shared_ptr<DrawRow>;

class DrawRow {
public:
	DrawRow(DynamicBuffer& buffer, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);
	DrawRow(TetrisEntry spriteEntry, DynamicBuffer& buffer, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);

	inline int getRow() const {
		return row_;
	}

	void handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void draw(float deltaTime, const BoardShader& shader);

	bool isAlive() const;

	inline void setSprites(mw::Sprite spriteI, mw::Sprite spriteJ, mw::Sprite spriteL, mw::Sprite spriteO, mw::Sprite spriteS, mw::Sprite spriteT, mw::Sprite spriteZ) {
		spriteI_ = spriteI;
		spriteJ_ = spriteJ;
		spriteL_ = spriteL;
		spriteO_ = spriteO;
		spriteS_ = spriteS;
		spriteT_ = spriteT;
		spriteZ_ = spriteZ;
	}

	VertexDataPtr getVertexData() const {
		return vd_;
	}

private:
	void init(DynamicBuffer& buffer, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);

	void updateVertexData(const TetrisBoard& tetrisBoard);
	void updateVertexData();

	mw::Sprite getSprite(BlockType blockType) const;

	int columns_;
	int row_;
	int oldRow_;
	float graphicRow_;
	float squareSize_;
	float lowX_, lowY_;
	float timeLeft_;

	float movingTime_;
	VertexDataPtr vd_;
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWROW_H
