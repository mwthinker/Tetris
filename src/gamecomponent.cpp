#include "gamecomponent.h"
#include "tetrisgame.h"
#include "gamefont.h"
#include "graphicboard.h"
#include "tetrisparameters.h"
#include "gamedata.h"

#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>

namespace {

	template <class A>
	void drawLineBorder(const A& a) {
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 0);
		glVertex2d(a.getWidth(), 0);
		glVertex2d(a.getWidth(), a.getHeight());
		glVertex2d(0, a.getHeight());
		glEnd();
	}

}

GameComponent::GameComponent(TetrisGame& tetrisGame, GameData& gameData) : tetrisGame_(tetrisGame), gameData_(gameData), alivePlayers_(0) {
	setGrabFocus(true);
	tetrisGame_.setGameHandler(this);
}

void GameComponent::draw(Uint32 deltaTime) {
	glPushMatrix();
	gui::Dimension dim = getSize();
	float width = 5;
	float height = 5;
	for (const auto& pair : graphic_) {
		width += (float) pair.second.getWidth() + 5;
		height = 5 + (float) pair.second.getHeight() + 5;
	}

	// Centers the game and holds the correct proportions.
	// The sides is transparent.
	if (width / dim.width_ > height / dim.height_) {
		// Blank sides, up and down.
		float scale = dim.width_ / width;
		glTranslatef(0, (dim.height_ - scale * height) * 0.5f, 0);
		glScalef(scale, scale, 1);
	} else {
		// Blank sides, left and right.
		float scale = dim.height_ / height;
		glTranslated((dim.width_ - scale * width) * 0.5f, 0, 0);
		glScalef(scale, scale, 1);
	}

	glTranslatef(5, 5, 0);
	for (auto& pair : graphic_) {
		if (tetrisGame_.isPaused()) {
			static mw::Text text("Paused", fontDefault30);
			pair.second.setMiddleMessage(text);
		}

		pair.second.draw();
		glTranslatef((float) pair.second.getWidth() + 5, 0, 0);
	}

	glPopMatrix();
}

void GameComponent::initGame(const std::vector<PlayerPtr>& players) {
	graphic_.clear();

	double width = 5;
	double height = 0;

	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	for (const PlayerPtr& player : players) {
		Graphic& graphic = graphic_[player->getId()];
		graphic = Graphic(player, showPoints, gameData_.spriteZ_,
			gameData_.spriteS_, gameData_.spriteJ_, gameData_.spriteI_,
			gameData_.spriteL_, gameData_.spriteT_, gameData_.spriteO_);
		width += graphic.getWidth() + 5;
		height = graphic.getHeight();
	}
	alivePlayers_ = players.size();
	setPreferredSize((float) width, (float) height);
}

void GameComponent::countDown(int msCountDown) {
	mw::Text text("", fontDefault30);
	if (msCountDown > 0) {
		std::stringstream stream;
		stream << "Start in " << (int) (msCountDown / 1000) + 1;
		text.setText(stream.str());
	}
	for (auto& pair : graphic_) {
		pair.second.setMiddleMessage(text);
	}
}

GameComponent::Graphic::Graphic() {
	name_ = mw::Text("", fontDefault30, 20);
}

GameComponent::Graphic::Graphic(const PlayerPtr& player, bool showPoints, mw::Sprite spriteZ, mw::Sprite spriteS, mw::Sprite spriteJ, mw::Sprite spriteI, mw::Sprite spriteL, mw::Sprite spriteT, mw::Sprite spriteO) : board_(player->getTetrisBoard()) {
	update(player);
	name_ = mw::Text(player->getName(), fontDefault30, 20);
	if (showPoints) {
		info_.showPoints();
	} else {
		info_.hidePoints();
	}
	// Sets the correct sprites.
	preview_.spriteZ_ = board_.spriteZ_ = spriteI;
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

void GameComponent::Graphic::draw() {
	glPushMatrix();

	glTranslated(5, 5, 0);
	board_.draw();
	mw::Color color(237 / 256.0, 78 / 256.0, 8 / 256.0);
	color.glColor3d();
	drawLineBorder(board_);

	glPushMatrix();
	glTranslated(board_.getWidth() + 5, board_.getHeight() - name_.getHeight(), 0);
	glColor3d(1, 1, 1);
	name_.draw();
	glTranslated(0, -5 - preview_.getHeight(), 0);
	preview_.draw();
	color.glColor3d();
	drawLineBorder(preview_);
	glPopMatrix();

	glPushMatrix();
	glTranslated(board_.getWidth() + 10, 10, 0);
	info_.draw();
	glPopMatrix();

	glPushMatrix();
	glTranslated(board_.getWidth() * 0.5 - middleMessage_.getWidth() * 0.5,
		board_.getHeight() * 0.5 - middleMessage_.getHeight() * 0.5, 0);
	middleMessage_.draw();
	glPopMatrix();

	glPopMatrix();

	glColor3d(1, 1, 1);
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
				mw::Text text("Game Over", fontDefault30);
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
				mw::Text text(stream.str(), fontDefault30);
				graphic_[player->getId()].setMiddleMessage(text);
			}
			break;
		default:
			break;
	}
}

void GameComponent::soundEffects(GameEvent gameEvent) const {
	mw::Sound sound;
	switch (gameEvent) {
		case GameEvent::GRAVITY_MOVES_BLOCK:
			break;
		case GameEvent::BLOCK_COLLISION:
			sound = gameData_.soundBlockCollision_;
			break;
		case GameEvent::PLAYER_ROTATES_BLOCK:
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through
		case GameEvent::THREE_ROW_REMOVED:
			sound = gameData_.soundRowRemoved_;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			sound = gameData_.soundTetris_;
			break;
		case GameEvent::GAME_OVER:
			break;
		default:
			break;
	}
	sound.play();
}
