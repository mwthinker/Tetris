#include "gamegraphic.h"
#include "tetrisboard.h"
#include "tetrisentry.h"

#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/text.h>
#include <mw/color.h>
#include <mw/opengl.h>
#include <gui/component.h>

#include <sstream>
#include <vector>
#include <cassert>

namespace {

	inline void addVertex(GLfloat* data, int& index,
		float x, float y,
		float xTex, float yTex,
		bool isTex,
		mw::Color color) {
		
		data[index++] = x;
		data[index++] = y;

		data[index++] = xTex;
		data[index++] = yTex;

		data[index++] = isTex;

		data[index++] = color.red_;
		data[index++] = color.green_;
		data[index++] = color.blue_;
		data[index++] = color.alpha_;
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(GLfloat* data, int& index,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		mw::Color color) {

		addVertex(data, index, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, index, x2, y2, xTex2, yTex2, isTex, color);
		addVertex(data, index, x3, y3, xTex3, yTex3, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Color color) {

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
	}

	inline void addSquare(GLfloat* data, int& index,
		float x, float y,
		float w, float h,
		mw::Sprite& sprite, mw::Color color = mw::Color(1,1,1)) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		// Left triangle |_
		addTriangle(data, index,
			x, y,
			x + w, y,
			x, y + h,
			sprite.getX() / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
		//                _
		// Right triangle  |
		addTriangle(data, index,
			x, y + h,
			x + w, y,
			x + w, y + h,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			color);
	}

	gui::Point calculateCenter(const Block& block) {
		gui::Point point(0, 0);
		for (const Square& sq : block) {
			point.x_ += sq.column_;
			point.y_ += sq.row_;
		}

		point.x_ /= block.nbrOfSquares();
		point.y_ /= block.nbrOfSquares();

		return point;
	}

} // Anonymous namespace.

GameGraphic::GameGraphic() {
}

GameGraphic::GameGraphic(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
	dynamicBoard_(x, y, boardEntry, tetrisBoard),
	staticBoard_(x, y, boardEntry, tetrisBoard),
	// sizeof [bytes/float] * 9 [floats/vertices] * 6 [vertices/square] * (rows * columns + 8) [squares].
	lowX_(x), lowY_(y)	{

	rows_ = tetrisBoard.getRows();
	columns_ = tetrisBoard.getColumns();

	font_ = boardEntry.getChildEntry("font").getFont(30);
	squareSize_ = boardEntry.getChildEntry("squareSize").getFloat();
	sizeBoard_ = squareSize_ * tetrisBoard.getColumns();

	// Define all text sizes and font usage.
	level_ = mw::Text("Level ", font_, 16);
	points_ = mw::Text("Points ", font_, 16);
	nbrOfClearedRows_ = mw::Text("Rows ", font_, 16);
	showPoints_ = true;

	name_ = mw::Text("Keyboard1 ", font_, 16);
}

void GameGraphic::update(const PlayerPtr& player) {
	//updateDynamicData(player->getTetrisBoard());
	PlayerInfo info = player->getPlayerInfo();
	update(info.nbrClearedRows_, info.points_, player->getLevel());
}

void GameGraphic::draw(float deltaTime, const BoardShader& shader) {
	staticBoard_.draw(deltaTime, shader);
	//dynamicBoard_.draw(deltaTime, shader);
}

void GameGraphic::drawText(float x, float y, float width, float height, float scale) {
	float boardWidth = 100 * scale;
	//name_.draw(x + boardWidth + borderSize_ * scale, height - y - name_.getHeight() - borderSize_ * scale);
	//points_.draw(x + boardWidth + borderSize_ * scale, y + 50 * scale + borderSize_ * scale);
	//level_.draw(x + boardWidth + borderSize_ * scale, y + 100 * scale + borderSize_ * scale);
	//nbrOfClearedRows_.draw(x + boardWidth + borderSize_ * scale, y + 10 * scale + borderSize_ * scale);
}

void GameGraphic::update(float size, const mw::Font& font) {
	points_ = mw::Text(points_.getText(), font, size);
	name_ = mw::Text(name_.getText(), font, size);
	level_ = mw::Text(level_.getText(), font, size);
	nbrOfClearedRows_ = mw::Text(nbrOfClearedRows_.getText(), font, size);
}

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameGraphic::update(int rowsCleared, int points, int level) {
	std::stringstream stream;
	stream << "Rows " << rowsCleared;
	nbrOfClearedRows_.setText(stream.str());
	stream.str("");
	stream << "Points " << points;
	points_.setText(stream.str());
	stream.str("");
	stream << "Level " << level;
	level_.setText(stream.str());
}

void GameGraphic::setName(std::string name) {
	name_.setText(name);
}
