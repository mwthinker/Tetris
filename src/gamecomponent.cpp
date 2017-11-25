#include "gamecomponent.h"
#include "tetrisgame.h"
#include "gamegraphic.h"
#include "tetrisparameters.h"
#include "tetrisgameevent.h"
#include "tetrisdata.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

GameComponent::GameComponent(TetrisGame& tetrisGame)
	: tetrisGame_(tetrisGame), alivePlayers_(0),
	updateMatrix_(true) {

	setGrabFocus(true);
	eventConnection_ = tetrisGame_.addGameEventHandler(std::bind(&GameComponent::eventHandler, this, std::placeholders::_1));

	soundBlockCollision_ = TetrisData::getInstance().getBlockCollisionSound();
	soundRowRemoved_ = TetrisData::getInstance().getRowRemovedSound();
	soundTetris_ = TetrisData::getInstance().getBlockCollisionSound();

	boardShader_ = std::make_shared<BoardShader>("board.ver.glsl", "board.fra.glsl");
	dynamicBoardBatch_ = std::make_shared<BoardBatch>(boardShader_, 10000);
}

GameComponent::~GameComponent() {
	eventConnection_.disconnect();
}

void GameComponent::validate() {
	updateMatrix_ = true;
}

void GameComponent::draw(const gui::Graphic& graphic, double deltaTime) {
	boardShader_->useProgram();
	const gui::Dimension dim = getSize();
	if (updateMatrix_) {
		float width = 0;
		float height = 0;

		for (const auto& pair : graphicPlayers_) {
			const GameGraphic& graphic = pair.second;
			width += graphic.getWidth();
			height = graphic.getHeight();
		}

		// Centers the game and holds the correct proportions.
		// The sides are transparent.
		Mat44 model = getModelMatrix();
		if (width / dim.width_ > height / dim.height_) {
			// Blank sides, up and down.
			scale_ = dim.width_ / width;
			dx_ = 0;
			dy_ = (dim.height_ - scale_ * height) * 0.5f;
		} else {
			// Blank sides, left and right.
			scale_ = dim.height_ / height;
			dx_ = (dim.width_ - scale_ * width) * 0.5f;
			dy_ = 0;
		}

		mw::translate2D(model, dx_, dy_);
		mw::scale2D(model, scale_, scale_);

		boardShader_->setMatrix(graphic.getProjectionMatrix() * model);
		boardShader_->useProgram();
		updateMatrix_ = false;
	}

	if (!graphicPlayers_.empty()) {
		// Draw boards.
		TetrisData::getInstance().bindTextureFromAtlas();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		dynamicBoardBatch_->clear();

		for (auto& pair : graphicPlayers_) {
			GameGraphic& graphic = pair.second;
			graphic.update((float) deltaTime, *dynamicBoardBatch_);
		}
		dynamicBoardBatch_->uploadToGraphicCard();

		staticBoardBatch_->draw();
		dynamicBoardBatch_->draw();

		// Draw text.
		for (auto& pair : graphicPlayers_) {
			GameGraphic& graphic = pair.second;
			graphic.drawText(*dynamicBoardBatch_);
		}

		mw::checkGlError();
	}
}

void GameComponent::initGame(std::vector<PlayerPtr>& players) {
	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	staticBoardBatch_ = std::make_shared<BoardBatch>(boardShader_);

	graphicPlayers_.clear();

	float w = 0;
	for (auto& player : players) {
		auto& graphic = graphicPlayers_[player];
		graphic.restart(*staticBoardBatch_, *player, w, 0);
		w += graphic.getWidth();
	}
	staticBoardBatch_->uploadToGraphicCard();

	alivePlayers_ = players.size();
	updateMatrix_ = true;
}

void GameComponent::countDown(int msCountDown) {
	mw::Text text("", TetrisData::getInstance().getInstance().getDefaultFont(30));
	if (msCountDown > 0) {
		std::stringstream stream;
		stream << "Start in " << (int) (msCountDown / 1000) + 1;
		text.setText(stream.str());
	}
}

void GameComponent::eventHandler(TetrisGameEvent& tetrisEvent) {
	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& initGameVar = dynamic_cast<InitGame&>(tetrisEvent);
		initGame(initGameVar.players_);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& levelChange = dynamic_cast<LevelChange&>(tetrisEvent);
		GameGraphic& gg = graphicPlayers_[levelChange.player_];
		gg.update(levelChange.player_->getClearedRows(), levelChange.player_->getPoints(), levelChange.newLevel_);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& pointsChange = dynamic_cast<PointsChange&>(tetrisEvent);
		GameGraphic& gg = graphicPlayers_[pointsChange.player_];
		gg.update(pointsChange.player_->getClearedRows(), pointsChange.player_->getPoints(), pointsChange.player_->getLevel());
		return;
	} catch (std::bad_cast exp) {}
}

/*
void GameComponent::eventHandler(const Player& player, PlayerEvent playerEvent) {
	GameGraphic& graphic = graphicPlayers_[player.getId()];

	graphic.update(player.getClearedRows(), player.getPoints(), player.getLevel());

	/*soundEffects(gameEvent);

	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			if (tetrisGame_.getNbrOfPlayers() == 1) {
				mw::Text text("Game Over", tetrisEntry_.getDeepChildEntry("window font").getFont(30));
				//it->second.setMiddleMessage(text);
			} else {
				std::stringstream stream;
				stream << alivePlayers_;
				if (alivePlayers_ == 1) {
					stream << ":st place!";
				} else if (alivePlayers_ == 2) {
					stream << ":nd place!";
				} else if (alivePlayers_ == 3) {
					stream << ":rd place!";
				} else {
					stream << ":th place!";
				}
				--alivePlayers_;
				//mw::Text text(stream.str(), tetrisEntry_.getEntry("window font").getFont(30));
				//it->second.setMiddleMessage(text);
			}
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through!
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through!
		case GameEvent::THREE_ROW_REMOVED:
			// Fall through!
		case GameEvent::FOUR_ROW_REMOVED:
			break;
	}
}*/

void GameComponent::soundEffects(GameEvent gameEvent) const {
	mw::Sound sound;
	switch (gameEvent) {
		case GameEvent::GRAVITY_MOVES_BLOCK:
			break;
		case GameEvent::BLOCK_COLLISION:
			sound = soundBlockCollision_;
			break;
		case GameEvent::PLAYER_MOVES_BLOCK_ROTATE:
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through
		case GameEvent::THREE_ROW_REMOVED:
			sound = soundRowRemoved_;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			sound = soundTetris_;
			break;
		case GameEvent::GAME_OVER:
			break;
	}
	sound.play();
}
