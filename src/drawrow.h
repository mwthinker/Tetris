#ifndef DRAWROW_H
#define DRAWROW_H

#include "boardvertexdata.h"
#include "tetrisentry.h"
#include "player.h"
#include "boardshader.h"

#include <mw/vertexbufferobject.h>

class DrawRow;
using DrawRowPtr = std::shared_ptr<DrawRow>;

class DrawRow : public BoardVertexData {
public:
	DrawRow(TetrisEntry spriteEntry, const BoardShader& boardShader, int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);

	inline int getRow() const {
		return row_;
	}

	void handleEvent(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	void draw(float deltaTime);

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

	void init(int row, const TetrisBoard& board, float squareSize, float lowX, float lowY);

private:
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
	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
};

#endif // DRAWROW_H
