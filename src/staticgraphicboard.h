#ifndef STATICGRAPHICBOARD_H
#define STATICGRAPHICBOARD_H

#include "tetrisboard.h"
#include "player.h"
#include "tetrisentry.h"
#include "boardshader.h"

#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/sprite.h>
#include <mw/shader.h>
#include <mw/vertexbufferobject.h>

#include <string>
#include <array>

class StaticGraphicBoard {
public:
	StaticGraphicBoard();

	StaticGraphicBoard(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void draw(float deltaTime, const BoardShader& shader);

private:
	void initStaticVbo(mw::Color c1, mw::Color c2, mw::Color c3, mw::Color c4, int columns, int rows);
		
	float squareSize_;
	float sizeBoard_;
	mw::VertexBufferObject staticVbo_;

	float lowX_, lowY_;
	float width_, height_;
	float borderSize_;
	mw::Color borderColor_;
	mw::Sprite borderHorizontal_, borderVertical_, borderLeftUp_, borderRightUp_, borderDownLeft_, borderDownRight_;

	int rows_, columns_;
	mw::Texture texture_;
};

#endif // STATICGRAPHICBOARD_H
