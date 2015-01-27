#ifndef GRAPHICBOARD_H
#define GRAPHICBOARD_H

#include "tetrisentry.h"
#include "boardshader.h"
#include "dynamicgraphicboard.h"
#include "staticgraphicboard.h"
#include "player.h"

#include <mw/font.h>
#include <mw/text.h>
#include <mw/sprite.h>
#include <mw/vertexbufferobject.h>
#include <mw/signal.h>

#include <string>

class TetrisBoard;

class GameGraphic {
public:
	~GameGraphic();

	void restart(Player& player, float x, float y,
		TetrisEntry boardEntry);
	
	void update(int clearedRows, int points, int level);

	void updateTextSize(float size, const mw::Font& font);

	void updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4) {
		dynamicBoard_.updateLinesRemoved(downTime, row1, row2, row3, row4);
	}

	inline float getWidth() const {
		return staticBoard_.getWidth();
	}

	inline float getHeight() const {
		return staticBoard_.getHeight();
	}

	void draw(float deltaTime, const BoardShader& shader);
	void drawText(float x, float y, float width, float height, float scale);

	void setMiddleMessage(const mw::Text& text);

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

	void setName(std::string name);

private:
	void callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard);

	DynamicGraphicBoard dynamicBoard_;
	StaticGraphicBoard staticBoard_;
	mw::Text textLevel_, textPoints_, textClearedRows_, name_, middleMessage_;
	int level_, points_, clearedRows_;

	mw::Font font_;
	mw::signals::Connection connection_;

	bool showPoints_;
};

#endif // GRAPHICBOARD_H
