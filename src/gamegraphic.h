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

#include <string>

class RawTetrisBoard;

class GameGraphic {
public:
	GameGraphic();

	GameGraphic(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard);

	void update(const PlayerPtr& player);

	inline float getWidth() const {
		return staticBoard_.getWidth();
	}

	inline float getHeight() const {
		return staticBoard_.getHeight();
	}

	void draw(float deltaTime, const BoardShader& shader);
	void drawText(float x, float y, float width, float height, float scale);

	void update(float size, const mw::Font& font);

	void setMiddleMessage(const mw::Text& text);

	inline void showPoints() {
		showPoints_ = true;
	}

	inline void hidePoints() {
		showPoints_ = false;
	}

	void setName(std::string name);

	void updateLinesRemoved(float downTime, int row1, int row2, int row3, int row4) {
	}

private:
	void update(int rowsCleared, int points, int level);

	DynamicGraphicBoard dynamicBoard_;
	StaticGraphicBoard staticBoard_;
	mw::Text level_, points_, nbrOfClearedRows_, name_, middleMessage_;
	mw::Font font_;

	bool showPoints_;
};

#endif // GRAPHICBOARD_H
