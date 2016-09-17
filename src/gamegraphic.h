#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisentry.h"
#include "boardshader.h"
#include "player.h"
#include "drawrow.h"
#include "vertexdata.h"
#include "dynamicbuffer.h"
#include "tetrisboard.h"
#include "boardshader.h"
#include "staticbuffer.h"
#include "drawblock.h"
#include "drawtext.h"

#include <mw/font.h>
#include <mw/text.h>
#include <mw/sprite.h>
#include <mw/vertexbufferobject.h>
#include <mw/signal.h>

#include <gui/component.h>

#include <string>

class GameGraphic {
public:
	~GameGraphic();

	void restart(Player& player, float x, float y, const TetrisEntry& boardEntry);

	void update(int clearedRows, int points, int level);

	void updateTextSize(float size, const mw::Font& font);

	inline float getWidth() const {
		return width_;
	}

	inline float getHeight() const {
		return height_;
	}

	void draw(float deltaTime, const BoardShader& shader);
	
	void setMiddleMessage(const mw::Text& text);

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

	void setName(std::string name);

	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

private:
	void initStaticBackground(float lowX, float lowY, const TetrisEntry& boardEntry, Player& player);

	void addDrawRowAtTheTop(const TetrisBoard& tetrisBoard, int nbr);

	VertexDataPtr staticVertexData_;
	DrawTextPtr textLevel_, textPoints_, textClearedRows_, name_, middleMessage_;
	int level_, points_, clearedRows_;

	mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	mw::Font font_;
	mw::signals::Connection connection_;

	float borderSize_, boardWidth_, width_, height_;
	bool showPoints_;

	std::list<DrawRowPtr> rows_;
	float squareSize_;
	float lowX_;
	float lowY_;

	DynamicBuffer buffer_;
	DrawBlockPtr currentBlockPtr_;
	DrawBlockPtr nextBlockPtr_;
};

#endif // GRAPHICBOARD_H
