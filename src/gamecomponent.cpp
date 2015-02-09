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

	soundBlockCollision_ = tetrisEntry_.getDeepChildEntry("window sounds blockCollision").getSound();
	soundRowRemoved_ = tetrisEntry_.getDeepChildEntry("window sounds rowRemoved").getSound();
	soundTetris_ = tetrisEntry_.getDeepChildEntry("window sounds tetris").getSound();

	boardShader_ = BoardShader("board.ver.glsl", "board.fra.glsl");
}

void GameComponent::validate() {
	updateMatrix_ = true;
}

void GameComponent::draw(Uint32 deltaTime) {
	setGlModelU(getModelMatrix());
	boardShader_.glUseProgram();

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
		mw::Matrix44 model = getModelMatrix();
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

		boardShader_.setGlMatrixU(getProjectionMatrix() * model);

		fontSize_ = scale_ * 12.f;
		if (font_.getCharacterSize() != fontSize_) {
			font_ = tetrisEntry_.getDeepChildEntry("window font").getFont((int) fontSize_);
			// Update the font!
			for (auto& pair : graphicPlayers_) {
				GameGraphic& graphic = pair.second;
				graphic.updateTextSize(fontSize_, font_);
			}
		}
		updateMatrix_ = false;
	}

	mw::glEnable(GL_BLEND);
	mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	mw::Text text; // Used to update the "Pause".

	// Draw boards.
	for (auto& pair : graphicPlayers_) {
		GameGraphic& graphic = pair.second;

		if (tetrisGame_.isPaused()) {
			text = mw::Text("Paused", tetrisEntry_.getDeepChildEntry("window font").getFont(30));
			graphic.setMiddleMessage(text);
		}

		graphic.draw(deltaTime / 1000.f, boardShader_);
	}

	// Draw texts.
	glUseProgram();

	int i = 0;
	float boardWidth = getSize().width_ / graphicPlayers_.size();
	for (auto& pair : graphicPlayers_) {
		GameGraphic& graphic = pair.second;
		graphic.drawText(*this, i * boardWidth + dx_, dy_, getSize().width_, getSize().height_, scale_);
		++i;
	}

	mw::glDisable(GL_BLEND);
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
		graphic.restart(*player, w, 0, tetrisEntry_.getDeepChildEntry("window tetrisBoard"));
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

void GameComponent::eventHandler(const PlayerPtr& player, GameEvent gameEvent) {
	GameGraphic& graphic = graphicPlayers_[player->getId()];
	graphic.update(player->getPlayerInfo().nbrClearedRows_, player->getPlayerInfo().points_, player->getLevel());

	soundEffects(gameEvent);
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
