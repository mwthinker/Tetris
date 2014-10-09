#include "gamecomponent.h"
#include "tetrisgame.h"
#include "gamegraphic.h"
#include "tetrisparameters.h"
#include "tetrisentry.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

GameComponent::GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry)
	: tetrisGame_(tetrisGame), tetrisEntry_(tetrisEntry), alivePlayers_(0),
	updateMatrix_(true) {

	setGrabFocus(true);
	tetrisGame_.setGameHandler(this);

	soundBlockCollision_ = tetrisEntry_.getEntry("window sounds blockCollision").getSound();
	soundRowRemoved_ = tetrisEntry_.getEntry("window sounds rowRemoved").getSound();
	soundTetris_ = tetrisEntry_.getEntry("window sounds tetris").getSound();
	
	// Must bind attributes before linking.
	boardShader_.bindAttribute("aPos");
	boardShader_.bindAttribute("aTex");
	boardShader_.bindAttribute("aIsTex");
	boardShader_.bindAttribute("aColor");
	boardShader_.loadAndLinkFromFile("board.ver.glsl", "board.fra.glsl");
	
	uMatIndex_ = boardShader_.getUniformLocation("uMat");
}

void GameComponent::validate() {
	updateMatrix_ = true;
}

void GameComponent::draw(Uint32 deltaTime) {
	auto wp = getWindowMatrixPtr();
	boardShader_.glUseProgram();

	if (updateMatrix_) {
		if (uMatIndex_ == -1) {
			uMatIndex_ = boardShader_.getUniformLocation("uMat");
		}

		gui::Dimension dim = getSize();
		gui::Dimension prefDim = getPreferredSize();


		float width = prefDim.width_;
		float height = prefDim.height_;

		// Centers the game and holds the correct proportions.
		// The sides is transparent.
		mw::Matrix44 model;
		if (width / dim.width_ > height / dim.height_) {
			// Blank sides, up and down.
			float scale = dim.width_ / width;
			model = mw::getTranslateMatrix44(2, (dim.height_ - scale * height) * 0.5f + 2)
				* mw::getScaleMatrix44(scale, scale);
		} else {
			// Blank sides, left and right.
			float scale = dim.height_ / height;
			model = mw::getTranslateMatrix44(2 + (dim.width_ - scale * width) * 0.5f, 2)
				* mw::getScaleMatrix44(scale, scale);
		}		

		mw::glUniformMatrix4fv(uMatIndex_, 1, false, (wp->getProjection() * wp->getModel() * model).data());
	}
	
	mw::glEnable(GL_BLEND);
	mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mw::glActiveTexture(GL_TEXTURE1);

	//graphic_.draw(boardShader_);
	for (auto& pair : graphicPlayers_) {
		if (tetrisGame_.isPaused()) {
			static mw::Text text("Paused", tetrisEntry_.getEntry("window font").getFont(30));
			pair.second.setMiddleMessage(text);
		}

		pair.second.draw(boardShader_);
		//pair.second.draw(wp);
		//model = model *  mw::getTranslateMatrix44(pair.second.getWidth() + 5, 0);
		//wp->setModel(model);
	}

	mw::glActiveTexture(GL_TEXTURE0);
	mw::glDisable(GL_BLEND);

	wp->useShader();
	mw::checkGlError();
}

void GameComponent::initGame(const std::vector<PlayerPtr>& players) {
	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	graphicPlayers_.clear();

	float width = 0;
	float height = 0;
	for (const auto& player : players) {
		graphicPlayers_[player->getId()] = GameGraphic(width, 0, tetrisEntry_.getEntry("window tetrisBoard"), player->getTetrisBoard());
		width += graphicPlayers_[player->getId()].getWidth();
		height = graphicPlayers_[player->getId()].getHeight();
	}
	
	alivePlayers_ = players.size();
	setPreferredSize(width, height);
}

void GameComponent::countDown(int msCountDown) {
	mw::Text text("", tetrisEntry_.getEntry("window font").getFont(30));
	if (msCountDown > 0) {
		std::stringstream stream;
		stream << "Start in " << (int) (msCountDown / 1000) + 1;
		text.setText(stream.str());
	}
}

void GameComponent::eventHandler(const PlayerPtr& player, GameEvent gameEvent) {
	GameGraphic& graphic = graphicPlayers_[player->getId()];

	graphic.update(player);
	soundEffects(gameEvent);
	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			if (tetrisGame_.getNbrOfPlayers() == 1) {
				mw::Text text("Game Over", tetrisEntry_.getEntry("window font").getFont(30));
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
	}
}

void GameComponent::soundEffects(GameEvent gameEvent) const {
	mw::Sound sound;
	switch (gameEvent) {
		case GameEvent::GRAVITY_MOVES_BLOCK:
			break;
		case GameEvent::BLOCK_COLLISION:
			sound = soundBlockCollision_;
			break;
		case GameEvent::PLAYER_ROTATES_BLOCK:
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
