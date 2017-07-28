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

void GameGraphic::restart(const LightningShader& lightningShader, const BoardShader& boardShader, Player& player, float x, float y) {
	level_ = -1;
	points_ = -1;
	clearedRows_ = -1;
	
	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&GameGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));

	initStaticBackground(lightningShader, boardShader, x, y, player);
	
	showPoints_ = true;

	removeRowSound_ = TetrisData::getInstance().getRowRemovedSound();

	update(player.getClearedRows(), player.getPoints(), player.getLevel());
}

void GameGraphic::initStaticBackground(const LightningShader& lightningShader, const BoardShader& boardShader, float lowX, float lowY, Player& player) {
	const TetrisBoard& tetrisBoard = player.getTetrisBoard();
	const Color c1 = TetrisData::getInstance().getOuterSquareColor();
	const Color c2 = TetrisData::getInstance().getInnerSquareColor();
	const Color c3 = TetrisData::getInstance().getStartAreaColor();
	const Color c4 = TetrisData::getInstance().getPlayerAreaColor();
	const Color borderColor = TetrisData::getInstance().getBorderColor();
	
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

	mw::Buffer staticBuffer(mw::Buffer::STATIC);
	staticVertexData_ = std::make_shared<BoardVertexData>(boardShader);
	staticBuffer.addVertexData(staticVertexData_);
	staticVertexData_->begin();

	lowX_ = lowX;
	lowY_ = lowY;
	
	// Draw the player area.
	float x = lowX + borderSize;
	float y = lowY * 0.5f + borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		boardWidth + infoSize + middleDistance + rightDistance, squareSize * (rows - 2),
		c4);

	// Draw the outer square.
	x = lowX + borderSize;
	y = lowY + borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize * columns, squareSize * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + borderSize + squareSize * column + squareSize * 0.1f;
			y = lowY + borderSize + squareSize * row + squareSize * 0.1f;
			staticVertexData_->addSquareTRIANGLES(
				x, y,
				squareSize * 0.8f, squareSize * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX + borderSize;
	y = lowY + borderSize + squareSize * (rows - 4);
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize * columns, squareSize * 2,
		c3);

	// Draw the preview block area.
	x = lowX + borderSize + boardWidth + middleDistance;
	y = lowY + borderSize + squareSize * (rows - 4) - (squareSize * 5 + middleDistance);
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		infoSize, infoSize,
		c3);
	
	nextBlockPtr_ = std::make_shared<DrawBlock>(boardShader, Block(tetrisBoard.getNextBlockType(), 0, 0), tetrisBoard.getRows(), squareSize, x + squareSize * 2.5f, y + squareSize * 2.5f, true);
	mw::Buffer dynamicBuffer(mw::Buffer::DYNAMIC);
	dynamicBuffer.addVertexData(nextBlockPtr_);
		
	mw::Font font = TetrisData::getInstance().getDefaultFont(30);

	name_ = std::make_shared<DrawText>(boardShader, player.getName(), font, x, y + squareSize * 5, 8.f);
	//name_->update("Marcus");
	dynamicBuffer.addVertexData(name_);
	
	{
		std::stringstream stream;
		level_ = player.getLevel();
		stream << "Level " << level_;
		textLevel_ = std::make_shared<DrawText>(boardShader, stream.str(), font, x, y - 20, 8.f);
		dynamicBuffer.addVertexData(textLevel_);
	}
	{
		std::stringstream stream;
		points_ = player.getPoints();
		stream << "Points " << points_;
		textPoints_ = std::make_shared<DrawText>(boardShader, stream.str(), font, x, y - 20 - 12, 8.f);
		dynamicBuffer.addVertexData(textPoints_);
	}
	{
		std::stringstream stream;
		clearedRows_ = player.getClearedRows();
		stream << "Rows " << clearedRows_;
		textClearedRows_ = std::make_shared<DrawText>(boardShader, stream.str(), font, x, y - 20 - 12 * 2, 8.f);
		dynamicBuffer.addVertexData(textClearedRows_);
	}

	// Add border.
	// Left-up corner.
	x = lowX;
	y = lowY + height_ - borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, borderSize,
		borderLeftUp,
		borderColor);

	// Right-up corner.
	x = lowX + width_ - borderSize;
	y = lowY + height_ - borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, borderSize,
		borderRightUp,
		borderColor);

	// Left-down corner.
	x = lowX;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, borderSize,
		borderDownLeft,
		borderColor);

	// Right-down corner.
	x = lowX + width_ - borderSize;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, borderSize,
		borderDownRight,
		borderColor);

	// Up.
	x = lowX + borderSize;
	y = lowY + height_ - borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		width_ - 2 * borderSize, borderSize,
		borderHorizontal,
		borderColor);

	// Down.
	x = lowX + borderSize;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		width_ - 2 * borderSize, borderSize,
		borderHorizontal,
		borderColor);

	// Left.
	x = lowX;
	y = lowY + borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, height_ - 2 * borderSize,
		borderVertical,
		borderColor);

	// Right.
	x = lowX + width_ - borderSize;
	y = lowY + borderSize;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize, height_ - 2 * borderSize,
		borderVertical,
		borderColor);	
	
	staticVertexData_->end();
	staticBuffer.uploadToGraphicCard();	
	
	rows_.clear();
	
	currentBlockPtr_ = std::make_shared<DrawBlock>(boardShader, tetrisBoard.getBlock(), tetrisBoard.getRows(), squareSize, lowX + borderSize, lowY + borderSize, false);
	dynamicBuffer.addVertexData(currentBlockPtr_);

	for (int row = 0; row < rows; ++row) {
		auto drawRow = std::make_shared<DrawRow>(boardShader, row, tetrisBoard, squareSize, lowX + borderSize, lowY + borderSize);
		dynamicBuffer.addVertexData(drawRow);
		rows_.push_back(drawRow);
	}
	
	dynamicBuffer.uploadToGraphicCard();

	std::vector<Vec2> points_;
	for (int i = 0; i < 11; ++i) {
		points_.push_back(Vec2(lowX+ (i + 1) * squareSize, 20 * squareSize));
	}

	mw::Sprite halfCircle = TetrisData::getInstance().getHalfCircleSprite();
	mw::Sprite lineSegment = TetrisData::getInstance().getLineSegmentSprite();

	lightningBoltCluster_ = LightningBoltCluster(lightningShader, halfCircle, lineSegment, points_, 50, 100, 100);
	//lightningBoltCluster_.restart(points_, 1);

	/*
	std::vector<Vec2> points;
	for (int column = 0; column < tetrisBoard.getColumns(); ++column) {
		BlockType type = tetrisBoard.getBlockType(row->getRow() - 1, column);
		points.push_back(Vec2(lowX_ + (column + 1) * squareSize_, lowY_ + (row->getRow() + 1) * squareSize_));
	}
	lightningBoltCluster_.restart(points, 5);
	*/
}

void GameGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	for (auto& row : rows_) {
		row->handleEvent(gameEvent, tetrisBoard);
	}
	rows_.remove_if([&](const DrawRowPtr& row) {
		if (!row->isAlive()) {
			freeRows_.push_back(row);
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
				auto drawRow = freeRows_.front();
				freeRows_.pop_front();
				drawRow->init(row, tetrisBoard);
				rows_.push_back(drawRow);
			}
		}
			break;
		case GameEvent::NEXT_BLOCK_UPDATED:
			if (nextBlockPtr_) {
				nextBlockPtr_->update(Block(tetrisBoard.getNextBlockType(), 0, 0));
			}
			break;
		case GameEvent::CURRENT_BLOCK_UPDATED:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_ROTATE:
			// Fall through!		
		case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_RIGHT:
			if (currentBlockPtr_) {
				currentBlockPtr_->update(tetrisBoard.getBlock());
			}
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN_GROUND:
			blockDownGround_ = true;
			latestBlockDownGround_ = tetrisBoard.getBlock();
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
			if (blockDownGround_) {
				currentBlockPtr_->updateDown(tetrisBoard.getBlock());
				int w = calculateWitdh(latestBlockDownGround_) + 1;
				int h = calculateHighest(latestBlockDownGround_);
				int lowest = tetrisBoard.getBlock().getLowestRow();
				int leftColumn = calculateLeftColumn(tetrisBoard.getBlock());

				std::vector<Vec2> points;
				//for (int row = lowest; row < h; ++row) {
				int row = lowest;
				for (int column = leftColumn; column < w + leftColumn; ++column) {
					Vec2 point(10 * (column + 1) + lowX_, 10 * (row + 2) + lowY_);
					points.push_back(point);
				}
				row = h;
				for (int column = leftColumn; column < w + leftColumn; ++column) {
					Vec2 point(10 * (column + 1) + lowX_, 10 * (row + 2) + lowY_);
					points.push_back(point);
				}
				//}
				lightningBoltCluster_.restart(points);
				blockDownGround_ = false;
			}
			// Fall through!
		case GameEvent::GRAVITY_MOVES_BLOCK:
			if (currentBlockPtr_) {
				currentBlockPtr_->update(tetrisBoard.getBlock());
			}
			break;
		case GameEvent::ROW_TO_BE_REMOVED:
		{
			std::stringstream stream;
			stream << "Rows " << tetrisBoard.getRemovedRows();
			textClearedRows_->update(stream.str());
		}
			break;
		case GameEvent::ONE_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 1);
			removeRowSound_.play();
			break;
		case GameEvent::TWO_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 2);
			removeRowSound_.play();
			break;
		case GameEvent::THREE_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 3);
			removeRowSound_.play();
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			addDrawRowAtTheTop(tetrisBoard, 4);
			removeRowSound_.play();
			break;
	}
}

void GameGraphic::addDrawRowAtTheTop(const TetrisBoard& tetrisBoard, int nbr) {
	int highestRow = tetrisBoard.getBoardVector().size() / tetrisBoard.getColumns();
	for (int i = 0; i < nbr; ++i) {
		auto drawRow = freeRows_.front();
		freeRows_.pop_front();
		drawRow->init(highestRow - i - 1, tetrisBoard);
		rows_.push_back(drawRow);
	}
}

void GameGraphic::draw(float deltaTime, GraphicMode mode) {
	switch (mode) {
		case GraphicMode::BOARD_SHADER:
			if (staticVertexData_) {
				staticVertexData_->drawTRIANGLES();
			}
			if (currentBlockPtr_) {
				currentBlockPtr_->update(deltaTime);
				currentBlockPtr_->drawTRIANGLES();
			}
			if (nextBlockPtr_) {
				nextBlockPtr_->drawTRIANGLES();
			}
			for (auto& rowPtr : rows_) {
				rowPtr->draw(deltaTime);
			}
			break;
		case GraphicMode::BOARD_SHADER_TEXT:
			if (name_) {
				name_->draw(deltaTime);
			}
			if (textLevel_) {
				textLevel_->draw(deltaTime);
			}
			if (textPoints_) {
				textPoints_->draw(deltaTime);
			}
			if (textClearedRows_) {
				textClearedRows_->draw(deltaTime);
			}
			break;
		case GraphicMode::LIGHTNING_SHADER:
			//lightningBoltCluster_.draw(deltaTime);
			break;
	}
}

void GameGraphic::updateTextSize(float size, const mw::Font& font) {
	//textPoints_ = mw::Text(textPoints_.getText(), font, size);
	//name_ = mw::Text(name_.getText(), font, size);
	//textLevel_ = mw::Text(textLevel_.getText(), font, size);
	//textClearedRows_ = mw::Text(textClearedRows_.getText(), font, size);
}

void GameGraphic::setMiddleMessage(const mw::Text& text) {
	//middleMessage_ = text;
}

void GameGraphic::update(int clearedRows, int points, int level) {
	if (clearedRows_ != clearedRows) {
		std::stringstream stream;
		clearedRows_ = clearedRows;
		stream << "Rows " << clearedRows;
		textClearedRows_->update(stream.str());
	}
	if (points_ != points) {
		std::stringstream stream;
		points_ = points;
		stream << "Points " << points;
		textPoints_->update(stream.str());
	}
	if (level_ != level) {
		std::stringstream stream;
		level_ = level;
		stream << "Level " << level;
		textLevel_->update(stream.str());
	}
}

void GameGraphic::setName(std::string name) {
	name_->update(name);
}
