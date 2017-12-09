#include "gamegraphic.h"
#include "tetrisboard.h"
#include "player.h"
#include "tetrisdata.h"

#include <sstream>
#include <iostream>
#include <limits>

namespace {

	int calculateWitdh(const Block& block) {
		int min = std::numeric_limits<int>::max();
		int max = 0;
		for (const Square& sq : block) {
			if (sq.column_ > max) {
				max = sq.column_;
			}
			if (sq.column_ < min) {
				min = sq.column_;
			}
		}
		return max + 1 - min;
	}

	int calculateHighest(const Block& block) {
		int max = 0;
		for (const Square& sq : block) {
			if (sq.row_ > max) {
				max = sq.row_;
			}
		}
		return max;
	}

	int calculateLeftColumn(const Block& block) {
		int min = std::numeric_limits<int>::max();
		for (const Square& sq : block) {
			if (sq.column_ < min) {
				min = sq.column_;
			}
		}
		return min;
	}

}

GameGraphic::GameGraphic() {
}

GameGraphic::~GameGraphic() {
	connection_.disconnect();
}

void GameGraphic::restart(BoardBatch& boardBatch, Player& player, float x, float y) {
	level_ = -1;
	points_ = -1;
	clearedRows_ = -1;

	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&GameGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));

	initStaticBackground(boardBatch, x, y, player);
	showPoints_ = true;

	update(player.getClearedRows(), player.getPoints(), player.getLevel());
}

void GameGraphic::initStaticBackground(BoardBatch& staticBoardBatch, float lowX, float lowY, Player& player) {
	const TetrisBoard& tetrisBoard = player.getTetrisBoard();
	const mw::Color c1 = TetrisData::getInstance().getOuterSquareColor();
	const mw::Color c2 = TetrisData::getInstance().getInnerSquareColor();
	const mw::Color c3 = TetrisData::getInstance().getStartAreaColor();
	const mw::Color c4 = TetrisData::getInstance().getPlayerAreaColor();
	const mw::Color borderColor = TetrisData::getInstance().getBorderColor();

	const mw::Sprite borderHorizontal = TetrisData::getInstance().getBorderHorizontalSprite();
	const mw::Sprite borderVertical = TetrisData::getInstance().getBorderVerticalSprite();
	const mw::Sprite borderLeftUp = TetrisData::getInstance().getBorderLeftUpSprite();
	const mw::Sprite borderRightUp = TetrisData::getInstance().getBorderRightUpSprite();
	const mw::Sprite borderDownLeft = TetrisData::getInstance().getBorderDownLeftSprite();
	const mw::Sprite borderDownRight = TetrisData::getInstance().getBorderDownRightSprite();

	const float squareSize = TetrisData::getInstance().getTetrisSquareSize();
	const float borderSize = TetrisData::getInstance().getTetrisBorderSize();

	const int columns = tetrisBoard.getColumns();
	const int rows = tetrisBoard.getRows();

	const float middleDistance = 5;
	const float rightDistance = 5;
	const float infoSize = squareSize * 5;
	const float boardWidth = squareSize * columns;

	width_ = squareSize * columns + infoSize + borderSize * 2 + middleDistance + rightDistance;
	height_ = squareSize * (rows - 2) + borderSize * 2;

	// Draw the player area.
	float x = lowX + borderSize;
	float y = lowY * 0.5f + borderSize;
	staticBoardBatch.addRectangle(
		x, y,
		boardWidth + infoSize + middleDistance + rightDistance, squareSize * (rows - 2),
		c4);

	// Draw the outer square.
	x = lowX + borderSize;
	y = lowY + borderSize;
	staticBoardBatch.addRectangle(
		x, y,
		squareSize * columns, squareSize * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + borderSize + squareSize * column + squareSize * 0.1f;
			y = lowY + borderSize + squareSize * row + squareSize * 0.1f;
			staticBoardBatch.addRectangle(
				x, y,
				squareSize * 0.8f, squareSize * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX + borderSize;
	y = lowY + borderSize + squareSize * (rows - 4);
	staticBoardBatch.addRectangle(
		x, y,
		squareSize * columns, squareSize * 2,
		c3);

	// Draw the preview block area.
	x = lowX + borderSize + boardWidth + middleDistance;
	y = lowY + borderSize + squareSize * (rows - 4) - (squareSize * 5 + middleDistance);
	staticBoardBatch.addRectangle(
		x, y,
		infoSize, infoSize,
		c3);

	nextBlock_ = DrawBlock(Block(tetrisBoard.getNextBlockType(), 0, 0), tetrisBoard.getRows(), squareSize, x + squareSize * 2.5f, y + squareSize * 2.5f, true);


	mw::Font font = TetrisData::getInstance().getDefaultFont(30);
	name_ = DrawText(player.getName(), font, x, y + squareSize * 5, 8.f);
	{
		std::stringstream stream;
		level_ = player.getLevel();
		stream << "Level " << level_;
		textLevel_ = DrawText(stream.str(), font, x, y - 20, 8.f);
	}
	{
		std::stringstream stream;
		points_ = player.getPoints();
		stream << "Points " << points_;
		textPoints_ = DrawText(stream.str(), font, x, y - 20 - 12, 8.f);
	}
	{
		std::stringstream stream;
		clearedRows_ = player.getClearedRows();
		stream << "Rows " << clearedRows_;
		textClearedRows_ = DrawText(stream.str(), font, x, y - 20 - 12 * 2, 8.f);
	}

	// Add border.
	// Left-up corner.
	x = lowX;
	y = lowY + height_ - borderSize;
	staticBoardBatch.addSquare(
		x, y,
		borderSize,
		borderLeftUp,
		borderColor);

	// Right-up corner.
	x = lowX + width_ - borderSize;
	y = lowY + height_ - borderSize;
	staticBoardBatch.addSquare(
		x, y,
		borderSize,
		borderRightUp,
		borderColor);

	// Left-down corner.
	x = lowX;
	y = lowY;
	staticBoardBatch.addSquare(
		x, y,
		borderSize,
		borderDownLeft,
		borderColor);

	// Right-down corner.
	x = lowX + width_ - borderSize;
	y = lowY;
	staticBoardBatch.addSquare(
		x, y,
		borderSize,
		borderDownRight,
		borderColor);

	// Up.
	x = lowX + borderSize;
	y = lowY + height_ - borderSize;
	staticBoardBatch.addRectangle(
		x, y,
		width_ - 2 * borderSize, borderSize,
		borderHorizontal,
		borderColor);

	// Down.
	x = lowX + borderSize;
	y = lowY;
	staticBoardBatch.addRectangle(
		x, y,
		width_ - 2 * borderSize, borderSize,
		borderHorizontal,
		borderColor);

	// Left.
	x = lowX;
	y = lowY + borderSize;
	staticBoardBatch.addRectangle(
		x, y,
		borderSize, height_ - 2 * borderSize,
		borderVertical,
		borderColor);

	// Right.
	x = lowX + width_ - borderSize;
	y = lowY + borderSize;
	staticBoardBatch.addRectangle(
		x, y,
		borderSize, height_ - 2 * borderSize,
		borderVertical,
		borderColor);

	rows_.clear();

	currentBlock_ = DrawBlock(tetrisBoard.getBlock(), tetrisBoard.getRows(), squareSize, lowX + borderSize, lowY + borderSize, false);

	// Add rows to represent the board.
	// Add free rows to represent potential rows, e.g. the board receives external rows.
	for (int row = 0; row < rows; ++row) {
		auto drawRow = std::make_shared<DrawRow>(row, tetrisBoard, squareSize, lowX + borderSize, lowY + borderSize);
		auto freeRow = std::make_shared<DrawRow>(row, tetrisBoard, squareSize, lowX + borderSize, lowY + borderSize);
		rows_.push_back(drawRow);
		freeRows_.push_back(freeRow);
	}

	middleText_ = DrawText(lowX + borderSize + squareSize * columns * 0.5f, lowY + height_ * 0.5f);
}

void GameGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	for (auto& row : rows_) {
		row->handleEvent(gameEvent, tetrisBoard);
	}
	rows_.remove_if([&](const DrawRowPtr& row) {
		if (!row->isAlive()) {
			freeRows_.push_front(row);
			return true;
		}
		return false;
	});
	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			break;
		case GameEvent::BLOCK_COLLISION:
			break;
		case GameEvent::RESTARTED:
			break;
		case GameEvent::EXTERNAL_ROWS_ADDED:
		{
			int rows = tetrisBoard.getNbrExternalRowsAdded();
			for (int row = 0; row < rows; ++row) {
				addDrawRowBottom(tetrisBoard, rows - row - 1);
			}
			int highestRow = tetrisBoard.getBoardVector().size() / tetrisBoard.getColumns();
			assert(rows_.size() - highestRow >= 0); // Something is wrong. Should not be posssible.
			for (int i = 0; i < (int) rows_.size() - highestRow; ++i) { // Remove unneeded empty rows at the top.
				rows_.pop_back();
			}
		}
		break;
		case GameEvent::NEXT_BLOCK_UPDATED:
			nextBlock_.update(Block(tetrisBoard.getNextBlockType(), 0, 0));
			break;
		case GameEvent::CURRENT_BLOCK_UPDATED:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_ROTATE:
			// Fall through!		
		case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_RIGHT:
			currentBlock_.update(tetrisBoard.getBlock());
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN_GROUND:
			blockDownGround_ = true;
			latestBlockDownGround_ = tetrisBoard.getBlock();
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
			if (blockDownGround_) {
				currentBlock_.updateDown(tetrisBoard.getBlock());
				blockDownGround_ = false;
			}
			// Fall through!
		case GameEvent::GRAVITY_MOVES_BLOCK:
			currentBlock_.update(tetrisBoard.getBlock());
			break;
		case GameEvent::ROW_TO_BE_REMOVED:
		{
			std::stringstream stream;
			stream << "Rows " << tetrisBoard.getRemovedRows();
			textClearedRows_.update(stream.str());
		}
		break;
		case GameEvent::ONE_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 1);
			break;
		case GameEvent::TWO_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 2);
			break;
		case GameEvent::THREE_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 3);
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 4);
			break;
	}
}

void GameGraphic::addDrawRowAtTheTop(const TetrisBoard& tetrisBoard, int nbr) {
	for (int i = 0; i < nbr; ++i) {
		addEmptyRowTop(tetrisBoard); // Add them in ascending order.
	}
}

void GameGraphic::update(float deltaTime, BoardBatch& dynamicBoardBatch) {
	currentBlock_.update(deltaTime);
	dynamicBoardBatch.add(currentBlock_.getVertexes());
	dynamicBoardBatch.add(nextBlock_.getVertexes());

	for (auto& rowPtr : freeRows_) {
		if (rowPtr->isActive()) {
			rowPtr->update(deltaTime);
			dynamicBoardBatch.add(rowPtr->getVertexes());
		}
	}

	for (auto& rowPtr : rows_) {
		rowPtr->update(deltaTime);
		dynamicBoardBatch.add(rowPtr->getVertexes());
	}
}

void GameGraphic::drawText(BoardBatch& batch) {
	batch.clear();
	name_.bindTexture();
	batch.add(name_.getVertexes());
	batch.uploadToGraphicCard();
	batch.draw();

	batch.clear();
	textPoints_.bindTexture();
	batch.add(textPoints_.getVertexes());
	batch.uploadToGraphicCard();
	batch.draw();

	batch.clear();
	textLevel_.bindTexture();
	batch.add(textLevel_.getVertexes());
	batch.uploadToGraphicCard();
	batch.draw();

	batch.clear();
	textClearedRows_.bindTexture();
	batch.add(textClearedRows_.getVertexes());
	batch.uploadToGraphicCard();
	batch.draw();
}

void GameGraphic::drawMiddleText(BoardBatch& batch) {
	if (!middleText_.isEmpty()) {
		middleText_.bindTexture();
		batch.clear();
		batch.add(middleText_.getVertexes());
		batch.uploadToGraphicCard();
		batch.draw();
	}
}

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	middleText_.update(text);
}

void GameGraphic::update(int clearedRows, int points, int level) {
	if (clearedRows_ != clearedRows) {
		std::stringstream stream;
		clearedRows_ = clearedRows;
		stream << "Rows " << clearedRows;
		textClearedRows_.update(stream.str());
	}
	if (points_ != points) {
		std::stringstream stream;
		points_ = points;
		stream << "Points " << points;
		textPoints_.update(stream.str());
	}
	if (level_ != level) {
		std::stringstream stream;
		level_ = level;
		stream << "Level " << level;
		textLevel_.update(stream.str());
	}
}

void GameGraphic::setName(std::string name) {
	name_.update(name);
}

void GameGraphic::addEmptyRowTop(const TetrisBoard& tetrisBoard) {
	assert(!freeRows_.empty()); // Should never be empty.
	if (!freeRows_.empty()) { // Just in case empty, but the game should be over anyway.
		auto drawRow = freeRows_.back();
		freeRows_.pop_back();
		drawRow->init(rows_.size(), tetrisBoard);
		rows_.push_back(drawRow);
	}
}

void GameGraphic::addDrawRowBottom(const TetrisBoard& tetrisBoard, int row) {
	assert(!freeRows_.empty()); // Should never be empty.
	if (!freeRows_.empty()) {
		auto drawRow = freeRows_.back();
		freeRows_.pop_back();
		drawRow->init(row, tetrisBoard);
		rows_.push_front(drawRow); // Add as the lowest row, i.e. on the bottom.
	}
}
