#include "gamegraphic.h"
#include "tetrisboard.h"
#include "player.h"

#include <mw/opengl.h>

#include <sstream>
#include <cassert>

GameGraphic::~GameGraphic() {
	connection_.disconnect();
}

void GameGraphic::restart(Player& player, float x, float y,
	TetrisEntry boardEntry) {

	dynamicBoard_.restart(x, y, boardEntry, player.getTetrisBoard());
	staticBoard_.restart(x, y, boardEntry, player.getTetrisBoard());

	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&GameGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));

	font_ = boardEntry.getChildEntry("font").getFont(30);

	// Define all text sizes and font usage.
	textLevel_ = mw::Text("Level ", font_, 16);
	textPoints_ = mw::Text("Points ", font_, 16);
	textClearedRows_ = mw::Text("Rows ", font_, 16);
	showPoints_ = true;

	name_ = mw::Text("Keyboard1 ", font_, 16);
}

void GameGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			break;
		case GameEvent::RESTARTED:
			dynamicBoard_.updateCurrentBlock(tetrisBoard.getBlock());
			dynamicBoard_.updatePreviewBlock(tetrisBoard.getBlockType());
			dynamicBoard_.updateBoard(tetrisBoard);
			break;
		case GameEvent::EXTERNAL_ROWS_ADDED:
			dynamicBoard_.updateBoard(tetrisBoard);
			break;
		case GameEvent::CURRENT_BLOCK_UPDATED:
			dynamicBoard_.updateBoard(tetrisBoard);
			break;
		case GameEvent::PLAYER_ROTATES_BLOCK:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_RIGHT:
			// Fall through!
		case GameEvent::GRAVITY_MOVES_BLOCK:
			dynamicBoard_.updateCurrentBlock(tetrisBoard.getBlock());
			break;
		case GameEvent::NEXT_BLOCK_UPDATED:
			dynamicBoard_.updatePreviewBlock(tetrisBoard.getNextBlockType());
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through!
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through!
		case GameEvent::THREE_ROW_REMOVED:
			// Fall through!
		case GameEvent::FOUR_ROW_REMOVED:
			dynamicBoard_.updateLinesRemoved(3.f,
				tetrisBoard.getRemovedRow1(),
				tetrisBoard.getRemovedRow2(),
				tetrisBoard.getRemovedRow3(),
				tetrisBoard.getRemovedRow4());
			break;
	}
}

void GameGraphic::draw(float deltaTime, const BoardShader& shader) {
	staticBoard_.draw(deltaTime, shader);
	dynamicBoard_.draw(deltaTime, shader);
}

void GameGraphic::drawText(float x, float y, float width, float height, float scale) {
	float boardWidth = staticBoard_.getBoardWidth() * scale;
	float borderSize = staticBoard_.getBorderSize();
	name_.draw(x + boardWidth + borderSize * scale, height - y - name_.getHeight() - borderSize * scale);
	textPoints_.draw(x + boardWidth + borderSize * scale, y + 50 * scale + borderSize * scale);
	textLevel_.draw(x + boardWidth + borderSize * scale, y + 100 * scale + borderSize * scale);
	textClearedRows_.draw(x + boardWidth + borderSize * scale, y + 10 * scale + boardWidth * scale);
}

void GameGraphic::updateTextSize(float size, const mw::Font& font) {
	textPoints_ = mw::Text(textPoints_.getText(), font, size);
	name_ = mw::Text(name_.getText(), font, size);
	textLevel_ = mw::Text(textLevel_.getText(), font, size);
	textClearedRows_ = mw::Text(textClearedRows_.getText(), font, size);
}

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameGraphic::update(int clearedRows, int points, int level) {
	if (clearedRows_ != clearedRows) {
		std::stringstream stream;
		clearedRows_ = clearedRows;
		stream << "Rows " << clearedRows;
		textClearedRows_.setText(stream.str());
	}
	if (points_ != points) {
		std::stringstream stream;
		level_ = level;
		stream << "Points " << points;
		textPoints_.setText(stream.str());
	}
	if (level_ != level) {
		std::stringstream stream;
		level_ = level;
		stream << "Level " << level;
		textLevel_.setText(stream.str());
	}
}

void GameGraphic::setName(std::string name) {
	name_.setText(name);
}
