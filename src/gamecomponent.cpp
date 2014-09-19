#include "gamecomponent.h"
#include "tetrisgame.h"
#include "graphicboard.h"
#include "tetrisparameters.h"
#include "tetrisentry.h"

#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>

namespace {

	template <class A>
	void drawLineBorder(const A& a) {
		/*
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 0);
		glVertex2d(a.getWidth(), 0);
		glVertex2d(a.getWidth(), a.getHeight());
		glVertex2d(0, a.getHeight());
		glEnd();
		*/
	}

}

GameComponent::GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry) : tetrisGame_(tetrisGame), tetrisEntry_(tetrisEntry), alivePlayers_(0) {
	setGrabFocus(true);
	tetrisGame_.setGameHandler(this);

	soundBlockCollision_ = tetrisEntry_.getEntry("window sounds blockCollision").getSound();
	soundRowRemoved_ = tetrisEntry_.getEntry("window sounds rowRemoved").getSound();
	soundTetris_ = tetrisEntry_.getEntry("window sounds tetris").getSound();

	spriteZ_ = tetrisEntry_.getEntry("window sprites squareZ").getSprite();
	spriteS_ = tetrisEntry_.getEntry("window sprites squareS").getSprite();
	spriteJ_ = tetrisEntry_.getEntry("window sprites squareJ").getSprite();
	spriteI_ = tetrisEntry_.getEntry("window sprites squareI").getSprite();
	spriteL_ = tetrisEntry_.getEntry("window sprites squareL").getSprite();
	spriteT_ = tetrisEntry_.getEntry("window sprites squareT").getSprite();
	spriteO_ = tetrisEntry_.getEntry("window sprites squareO").getSprite();
}

void GameComponent::draw(Uint32 deltaTime) {
	auto wp = getWindowMatrixPtr();
	wp->useShader();
	auto old = wp->getModel();

	gui::Dimension dim = getSize();
	float width = 5;
	float height = 5;
	for (const auto& pair : graphic_) {
		width += pair.second.getWidth() + 5;
		height = 5 + pair.second.getHeight() + 5;
	}

	// Centers the game and holds the correct proportions.
	// The sides is transparent.
	mw::Matrix44 model = old;
	if (width / dim.width_ > height / dim.height_) {
		// Blank sides, up and down.
		float scale = dim.width_ / width;
		model = model * mw::getTranslateMatrix44(5, (dim.height_ - scale * height) * 0.5f + 5) * mw::getScaleMatrix44(scale, scale);
	} else {
		// Blank sides, left and right.
		float scale = dim.height_ / height;
		model = model * mw::getTranslateMatrix44(5 + (dim.width_ - scale * width) * 0.5f, 5) * mw::getScaleMatrix44(scale, scale);
	}
	wp->setModel(model);
	for (auto& pair : graphic_) {
		if (tetrisGame_.isPaused()) {
			static mw::Text text("Paused", tetrisEntry_.getEntry("window font").getFont(30));
			pair.second.setMiddleMessage(text);
		}

		pair.second.draw(wp);
		model = model *  mw::getTranslateMatrix44(pair.second.getWidth() + 5, 0);
		wp->setModel(model);
	}
	wp->setModel(old);
}

void GameComponent::initGame(const std::vector<PlayerPtr>& players) {
	graphic_.clear();

	float width = 5;
	float height = 0;

	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	for (const PlayerPtr& player : players) {
		Graphic& graphic = graphic_[player->getId()];
		graphic = Graphic(player, showPoints, spriteZ_, 
			spriteS_, spriteJ_, spriteI_, 
			spriteL_, spriteT_, spriteO_,
			tetrisEntry_.getEntry("window font").getFont(30));
		width += graphic.getWidth() + 5;
		height = graphic.getHeight();
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
	for (auto& pair : graphic_) {
		pair.second.setMiddleMessage(text);
	}
}

GameComponent::Graphic::Graphic() : info_(mw::Font()) {
	name_ = mw::Text("", font_, 20);
}

GameComponent::Graphic::Graphic(const PlayerPtr& player, bool showPoints, mw::Sprite spriteZ, mw::Sprite spriteS, mw::Sprite spriteJ, mw::Sprite spriteI, mw::Sprite spriteL, mw::Sprite spriteT, mw::Sprite spriteO, const mw::Font& font) : board_(player->getTetrisBoard()), info_(font), font_(font) {
	update(player);
	name_ = mw::Text(player->getName(), font_, 20);
	if (showPoints) {
		info_.showPoints();
	} else {
		info_.hidePoints();
	}
	// Set the correct sprites.
	preview_.spriteZ_ = board_.spriteZ_ = spriteZ;
	preview_.spriteS_ = board_.spriteS_ = spriteS;
	preview_.spriteJ_ = board_.spriteJ_ = spriteJ;
	preview_.spriteI_ = board_.spriteI_ = spriteI;
	preview_.spriteL_ = board_.spriteL_ = spriteL;
	preview_.spriteT_ = board_.spriteT_ = spriteT;
	preview_.spriteO_ = board_.spriteO_ = spriteO;
}

void GameComponent::Graphic::update(const PlayerPtr& player) {
	PlayerInfo& info = player->getPlayerInfo();
	info_.update(info.nbrClearedRows_, info.points_, player->getLevel());
	board_.update();
	preview_.update(player->getNextBlock(), board_.getPixelPerSquare());
}

float GameComponent::Graphic::getWidth() const {
	return 5 + board_.getWidth() + 5 + preview_.getWidth() + 5;
}

float GameComponent::Graphic::getHeight() const {
	return 5 + board_.getHeight() + 5;
}

void GameComponent::Graphic::draw(gui::WindowMatrixPtr wp) {
	wp->useShader();
	auto old = wp->getModel();
	wp->setModel(old * mw::getTranslateMatrix44(5, 5));
	board_.draw(wp);
	mw::Color color(237 / 256.f, 78 / 256.f, 8 / 256.f);
	wp->setColor(color);
	drawLineBorder(board_);
	auto old2 = wp->getModel();
	wp->setModel(old2 * mw::getTranslateMatrix44(board_.getWidth() + 5, board_.getHeight() - name_.getHeight()));
	wp->setColor(1, 1, 1);
	name_.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(0, -5 - preview_.getHeight()));
	preview_.draw(wp);
	wp->setColor(color);
	drawLineBorder(preview_);
	wp->setModel(old2);

	old2 = wp->getModel();
	wp->setModel(old2 * mw::getTranslateMatrix44(board_.getWidth() + 10, 10));
	info_.draw(wp);
	wp->setModel(old2);

	old2 = wp->getModel();
	wp->setModel(old2 * mw::getTranslateMatrix44(board_.getWidth() * 0.5f - middleMessage_.getWidth() * 0.5f,
		board_.getHeight() * 0.5f - middleMessage_.getHeight() * 0.5f));
	middleMessage_.draw();
	wp->setModel(old);
	wp->setColor(1, 1, 1);
	drawLineBorder(*this);
}

void GameComponent::Graphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameComponent::eventHandler(const PlayerPtr& player, GameEvent gameEvent) {
	graphic_[player->getId()].update(player);
	soundEffects(gameEvent);
	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			if (tetrisGame_.getNbrOfPlayers() == 1) {
				mw::Text text("Game Over", tetrisEntry_.getEntry("window font").getFont(30));
				graphic_[player->getId()].setMiddleMessage(text);
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
				mw::Text text(stream.str(), tetrisEntry_.getEntry("window font").getFont(30));
				graphic_[player->getId()].setMiddleMessage(text);
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
