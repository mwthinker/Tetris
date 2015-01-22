#ifndef STATICGRAPHICBOARD_H
#define STATICGRAPHICBOARD_H

#include "tetrisentry.h"

#include <mw/vertexbufferobject.h>

class RawTetrisBoard;
class BoardShader;

class StaticGraphicBoard {
public:
	StaticGraphicBoard();

	StaticGraphicBoard(float lowX, float lowY, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void draw(float deltaTime, const BoardShader& shader);

	float getWidth() const;

	float getHeight() const;

	float getBoardWidth() const;
	
	float getBorderSize() const;

private:
	mw::VertexBufferObject staticVbo_;
	
	float width_, height_;
	float borderSize_;
	float boardWidth_;
	
};

#endif // STATICGRAPHICBOARD_H
