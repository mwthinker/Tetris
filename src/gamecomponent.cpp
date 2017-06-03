#include "gamecomponent.h"
#include "tetrisgame.h"
#include "gamegraphic.h"
#include "tetrisparameters.h"
#include "tetrisentry.h"
#include "tetrisgameevent.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

GameComponent::GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry)
	: tetrisGame_(tetrisGame), tetrisEntry_(tetrisEntry), alivePlayers_(0),
	updateMatrix_(true), buffer_(mw::Buffer::STATIC) {

	setGrabFocus(true);
	eventConnection_ = tetrisGame_.addGameEventHandler(std::bind(&GameComponent::eventHandler, this, std::placeholders::_1));

	soundBlockCollision_ = tetrisEntry_.getDeepChildEntry("window sounds blockCollision").getSound();
	soundRowRemoved_ = tetrisEntry_.getDeepChildEntry("window sounds rowRemoved").getSound();
	soundTetris_ = tetrisEntry_.getDeepChildEntry("window sounds tetris").getSound();
	
	boardShader_ = BoardShader("board.ver.glsl", "board.fra.glsl");
	lightningShader_ = LightningShader("lightning.ver.glsl", "lightning.fra.glsl");
}

GameComponent::~GameComponent() {
	eventConnection_.disconnect();
}

void GameComponent::validate() {
	updateMatrix_ = true;
}

void GameComponent::draw(const gui::Graphic& graphic, double deltaTime) {
	boardShader_.useProgram();
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
		// The sides is transparent.
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

		boardShader_.setGlMatrixU(graphic.getProjectionMatrix() * model);
		lightningShader_.useProgram();
		lightningShader_.setUMat(graphic.getProjectionMatrix() * model);
		lightningShader_.setUColor(Color(1, 1, 1));
		boardShader_.useProgram();
		updateMatrix_ = false;
	}

	mw::Text text; // Used to update the "Pause".
	
	// Draw boards.
	tetrisEntry_.bindTextureFromAtlas();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	for (auto& pair : graphicPlayers_) {
		GameGraphic& graphic = pair.second;
		graphic.draw((float) deltaTime, GameGraphic::BOARD_SHADER);
	}

	// Draw text.
	for (auto& pair : graphicPlayers_) {
		GameGraphic& graphic = pair.second;
		graphic.draw((float) deltaTime, GameGraphic::BOARD_SHADER_TEXT);
	}

	for (auto& pair : graphicPlayers_) {
		GameGraphic& graphic = pair.second;
		graphic.draw((float) deltaTime, GameGraphic::LIGHTNING_SHADER);
	}
	
	mw::checkGlError();
}

void GameComponent::initGame(std::vector<PlayerPtr>& players) {
	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	graphicPlayers_.clear();

	float w = 0;
	for (auto& player : players) {
		auto& graphic = graphicPlayers_[player->getId()];
		graphic.restart(lightningShader_, boardShader_, *player, w, 0, tetrisEntry_.getDeepChildEntry("window"));
		w += graphic.getWidth();
	}

	alivePlayers_ = players.size();
	updateMatrix_ = true;
}


void GameComponent::countDown(int msCountDown) {
	mw::Text text("", tetrisEntry_.getDeepChildEntry("window font").getFont(30));
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
		GameGraphic& gg = graphicPlayers_[levelChange.player_->getId()];
		gg.update(levelChange.player_->getPoints(), levelChange.player_->getPoints(), levelChange.newLevel_);
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
