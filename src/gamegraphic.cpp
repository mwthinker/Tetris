#include "gamegraphic.h"
#include "tetrisboard.h"
#include "player.h"

#include <sstream>
#include <iostream>

GameGraphic::GameGraphic() : dynamicBuffer_(false), dynamicBufferLightning_(false) {
}

GameGraphic::~GameGraphic() {
	connection_.disconnect();
}

void GameGraphic::restart(const LightningShader& lightningShader, const BoardShader& boardShader, Player& player, float x, float y,
	const TetrisEntry& boardEntry) {

	level_ = -1;
	points_ = -1;
	clearedRows_ = -1;
	
	connection_.disconnect();
	connection_ = player.addGameEventListener(std::bind(&GameGraphic::callback, this, std::placeholders::_1, std::placeholders::_2));

	font_ = boardEntry.getChildEntry("font").getFont(30);

	initStaticBackground(lightningShader, boardShader, x, y, boardEntry, player);
	
	showPoints_ = true;

	removeRowSound_ = boardEntry.getDeepChildEntry("sounds rowRemoved").getSound();

	update(player.getClearedRows(), player.getPoints(), player.getLevel());
}

void GameGraphic::initStaticBackground(const LightningShader& lightningShader, const BoardShader& boardShader, float lowX, float lowY,
	const TetrisEntry& windowEntry, Player& player) {

	lowX_ = lowX;
	lowY_ = lowY;

	const TetrisBoard& tetrisBoard = player.getTetrisBoard();

	const Color c1 = windowEntry.getDeepChildEntry("tetrisBoard outerSquareColor").getColor();
	const Color c2 = windowEntry.getDeepChildEntry("tetrisBoard innerSquareColor").getColor();
	const Color c3 = windowEntry.getDeepChildEntry("tetrisBoard startAreaColor").getColor();
	const Color c4 = windowEntry.getDeepChildEntry("tetrisBoard playerAreaColor").getColor();
	const Color borderColor = windowEntry.getDeepChildEntry("tetrisBoard borderColor").getColor();

	auto spriteEntry = windowEntry.getDeepChildEntry("tetrisBoard sprites");
	const mw::Sprite borderHorizontal = spriteEntry.getChildEntry("borderHorizontal").getSprite();
	const mw::Sprite borderVertical = spriteEntry.getChildEntry("borderVertical").getSprite();
	const mw::Sprite borderLeftUp = spriteEntry.getChildEntry("borderLeftUp").getSprite();
	const mw::Sprite borderRightUp = spriteEntry.getChildEntry("borderRightUp").getSprite();
	const mw::Sprite borderDownLeft = spriteEntry.getChildEntry("borderDownLeft").getSprite();
	const mw::Sprite borderDownRight = spriteEntry.getChildEntry("borderDownRight").getSprite();

	squareSize_ = windowEntry.getDeepChildEntry("tetrisBoard squareSize").getFloat();
	borderSize_ = windowEntry.getDeepChildEntry("tetrisBoard borderSize").getFloat();

	spriteZ_ = spriteEntry.getChildEntry("squareZ").getSprite();
	spriteS_ = spriteEntry.getChildEntry("squareS").getSprite();
	spriteJ_ = spriteEntry.getChildEntry("squareJ").getSprite();
	spriteI_ = spriteEntry.getChildEntry("squareI").getSprite();
	spriteL_ = spriteEntry.getChildEntry("squareL").getSprite();
	spriteT_ = spriteEntry.getChildEntry("squareT").getSprite();
	spriteO_ = spriteEntry.getChildEntry("squareO").getSprite();

	const int columns = tetrisBoard.getColumns();
	const int rows = tetrisBoard.getRows();

	const float infoSize = 70;
	boardWidth_ = squareSize_ * columns;

	width_ = squareSize_ * columns + infoSize + borderSize_ * 2;
	height_ = squareSize_ * (rows - 2) + borderSize_ * 2;

	mw::Buffer staticBuffer(true);
	staticVertexData_ = std::make_shared<BoardVertexData>(boardShader);
	staticBuffer.addVertexData(staticVertexData_);
	staticVertexData_->begin();
	
	// Draw the player area.
	float x = lowX + borderSize_;
	float y = lowY * 0.5f + borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		boardWidth_ + infoSize, squareSize_ * (rows - 2),
		c4);

	// Draw the outer square.
	x = lowX + borderSize_;
	y = lowY + borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize_ * columns, squareSize_ * (rows - 2),
		c1);

	// Draw the inner squares.
	for (int row = 0; row < rows - 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + borderSize_ + squareSize_ * column + squareSize_ * 0.1f;
			y = lowY + borderSize_ + squareSize_ * row + squareSize_ * 0.1f;
			staticVertexData_->addSquareTRIANGLES(
				x, y,
				squareSize_ * 0.8f, squareSize_ * 0.8f,
				c2);
		}
	}

	// Draw the block start area.
	x = lowX + borderSize_;
	y = lowY + borderSize_ + squareSize_ * (rows - 4);
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize_ * columns, squareSize_ * 2,
		c3);

	// Draw the preview block area.
	x = lowX + borderSize_ + boardWidth_ + 5;
	y = lowY + borderSize_ + squareSize_ * (rows - 4) - (squareSize_ * 5 + 5);
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		squareSize_ * 5, squareSize_ * 5,
		c3);
	
	nextBlockPtr_ = std::make_shared<DrawBlock>(boardShader, spriteEntry, Block(tetrisBoard.getNextBlockType(), 0, 0), tetrisBoard.getRows(), squareSize_, x + squareSize_ * 2.5f, y + squareSize_ * 2.5f, true);
	dynamicBuffer_.addVertexData(nextBlockPtr_);
		
	name_ = std::make_shared<DrawText>(boardShader, player.getName(), font_, x, y + squareSize_ * 5, 10.f);
	name_->update("Marcus");
	dynamicBuffer_.addVertexData(name_);
	
	textLevel_ = std::make_shared<DrawText>(boardShader, "1", font_, x, y - 20, 10.f);
	dynamicBuffer_.addVertexData(textLevel_);

	textPoints_ = std::make_shared<DrawText>(boardShader, "0", font_, x, y - 40, 10.f);
	dynamicBuffer_.addVertexData(textPoints_);
		
	textClearedRows_ = std::make_shared<DrawText>(boardShader, "0", font_, x, y - 60, 10.f);
	dynamicBuffer_.addVertexData(textClearedRows_);

	// Add border.
	// Left-up corner.
	x = lowX;
	y = lowY + height_ - borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, borderSize_,
		borderLeftUp,
		borderColor);

	// Right-up corner.
	x = lowX + width_ - borderSize_;
	y = lowY + height_ - borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, borderSize_,
		borderRightUp,
		borderColor);

	// Left-down corner.
	x = lowX;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, borderSize_,
		borderDownLeft,
		borderColor);

	// Right-down corner.
	x = lowX + width_ - borderSize_;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, borderSize_,
		borderDownRight,
		borderColor);

	// Up.
	x = lowX + borderSize_;
	y = lowY + height_ - borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal,
		borderColor);

	// Down.
	x = lowX + borderSize_;
	y = lowY;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		width_ - 2 * borderSize_, borderSize_,
		borderHorizontal,
		borderColor);

	// Left.
	x = lowX;
	y = lowY + borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical,
		borderColor);

	// Right.
	x = lowX + width_ - borderSize_;
	y = lowY + borderSize_;
	staticVertexData_->addSquareTRIANGLES(
		x, y,
		borderSize_, height_ - 2 * borderSize_,
		borderVertical,
		borderColor);	
	
	staticVertexData_->end();
	staticBuffer.uploadToGraphicCard();	
	
	rows_.clear();
	
	currentBlockPtr_ = std::make_shared<DrawBlock>(boardShader, spriteEntry, tetrisBoard.getBlock(), tetrisBoard.getRows(), squareSize_, lowX, lowY, false);
	dynamicBuffer_.addVertexData(currentBlockPtr_);

	for (int row = 0; row < rows; ++row) {
		auto drawRow = std::make_shared<DrawRow>(spriteEntry, boardShader, row, tetrisBoard, squareSize_, lowX, lowY);
		dynamicBuffer_.addVertexData(drawRow);
		rows_.push_back(drawRow);
	}

	for (int row = 0; row < rows; ++row) {
		auto drawRow = std::make_shared<DrawRow>(spriteEntry, boardShader, row, tetrisBoard, squareSize_, lowX, lowY);
		dynamicBuffer_.addVertexData(drawRow);
		freeRows_.push_back(drawRow);
	}
	
	dynamicBuffer_.uploadToGraphicCard();
	dynamicBuffer_.clearBuffer();

	lightningShader.useProgram();
	lightningBolt_ = std::make_shared<LightningBolt>(lightningShader, Vec2(50, 50), Vec2(100, 200),
		spriteEntry.getChildEntry("halfCircle").getSprite(),
		spriteEntry.getChildEntry("lineSegment").getSprite());

	dynamicBufferLightning_.addVertexData(lightningBolt_);
	dynamicBufferLightning_.uploadToGraphicCard();
	dynamicBufferLightning_.clearBuffer();
}

void GameGraphic::callback(GameEvent gameEvent, const TetrisBoard& tetrisBoard) {
	for (auto row : rows_) {
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
				drawRow->init(row, tetrisBoard, squareSize_, lowX_, lowY_);
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
		case GameEvent::PLAYER_ROTATES_BLOCK:
			// Fall through!		
		case GameEvent::PLAYER_MOVES_BLOCK_LEFT:
			// Fall through!
		case GameEvent::PLAYER_MOVES_BLOCK_RIGHT:
			if (currentBlockPtr_) {
				currentBlockPtr_->update(tetrisBoard.getBlock());
			}
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_DOWN:
			// Fall through!
		case GameEvent::GRAVITY_MOVES_BLOCK:
			if (currentBlockPtr_) {
				currentBlockPtr_->update(tetrisBoard.getBlock());
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
		drawRow->init(highestRow - i - 1, tetrisBoard, squareSize_, lowX_, lowY_);
		rows_.push_back(drawRow);
	}
}

void GameGraphic::draw(float deltaTime, GraphicMode mode) {
	//const LightningShader& lightningShader, const BoardShader& shader) {
	//shader.useProgram();
	switch (mode) {
		case GraphicMode::BOARD_SHADER:
			if (staticVertexData_) {
				staticVertexData_->drawTRIANGLES();
			}
			if (currentBlockPtr_) {
				currentBlockPtr_->drawTRIANGLES();
			}
			if (nextBlockPtr_) {
				nextBlockPtr_->drawTRIANGLES();
			}
			for (DrawRowPtr rowPtr : rows_) {
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
			if (lightningBolt_) {
				lightningBolt_->draw(deltaTime);
			}
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
	/*
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
	*/
}

void GameGraphic::setName(std::string name) {
	name_->update(name);
}
