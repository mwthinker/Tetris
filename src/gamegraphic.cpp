#include "gamegraphic.h"
#include "rawtetrisboard.h"

#include <mw/opengl.h>

#include <sstream>
#include <cassert>

GameGraphic::GameGraphic() {
}

GameGraphic::GameGraphic(float x, float y, TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) :
	dynamicBoard_(x, y, boardEntry, tetrisBoard),
	staticBoard_(x, y, boardEntry, tetrisBoard) {

	font_ = boardEntry.getChildEntry("font").getFont(30);

	// Define all text sizes and font usage.
	level_ = mw::Text("Level ", font_, 16);
	points_ = mw::Text("Points ", font_, 16);
	nbrOfClearedRows_ = mw::Text("Rows ", font_, 16);
	showPoints_ = true;

	name_ = mw::Text("Keyboard1 ", font_, 16);
}

void GameGraphic::update(const PlayerPtr& player) {
	dynamicBoard_.update(player);
	PlayerInfo info = player->getPlayerInfo();
	update(info.nbrClearedRows_, info.points_, player->getLevel());
}

void GameGraphic::draw(float deltaTime, const BoardShader& shader) {
	staticBoard_.draw(deltaTime, shader);
	dynamicBoard_.draw(deltaTime, shader);
}

void GameGraphic::drawText(float x, float y, float width, float height, float scale) {
	float boardWidth = staticBoard_.getBoardWidth() * scale;
	float borderSize = staticBoard_.getBorderSize();
	name_.draw(x + boardWidth + borderSize * scale, height - y - name_.getHeight() - borderSize * scale);
	points_.draw(x + boardWidth + borderSize * scale, y + 50 * scale + borderSize * scale);
	level_.draw(x + boardWidth + borderSize * scale, y + 100 * scale + borderSize * scale);
	nbrOfClearedRows_.draw(x + boardWidth + borderSize * scale, y + 10 * scale + boardWidth * scale);
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
