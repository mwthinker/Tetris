#include "gamecomponent.h"
#include "tetrisgame.h"
#include "graphicboard.h"
#include "tetrisparameters.h"
#include "tetrisentry.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

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
	
	// Must bind attributes before linking.
	boardShader_.bindAttribute("aPos");
	boardShader_.bindAttribute("aColor");
	boardShader_.loadAndLinkFromFile("board.ver.glsl", "board.fra.glsl");
}

void GameComponent::draw(Uint32 deltaTime) {
	gui::Dimension dim = getSize();

	float width = 400;
	float height = 400;

	// Centers the game and holds the correct proportions.
	// The sides is transparent.
	mw::Matrix44 model;
	if (width / dim.width_ > height / dim.height_) {
		// Blank sides, up and down.
		float scale = dim.width_ / width;
		model = mw::getTranslateMatrix44(5, (dim.height_ - scale * height) * 0.5f + 5) * mw::getScaleMatrix44(scale, scale);
	} else {
		// Blank sides, left and right.
		float scale = dim.height_ / height;
		model = mw::getTranslateMatrix44(5 + (dim.width_ - scale * width) * 0.5f, 5) * mw::getScaleMatrix44(scale, scale);
	}

	auto wp = getWindowMatrixPtr();
	mw::Matrix44 m = wp->getProjection() * wp->getModel() * model;

	boardShader_.glUseProgram();
	mw::glUniformMatrix4fv(boardShader_.getUniformLocation("uMat"), 1, false, m.data());
	mw::glEnable(GL_BLEND);
	mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	graphic_.draw(boardShader_);
	mw::glDisable(GL_BLEND);

	/*
	for (auto& pair : graphic_) {
		if (tetrisGame_.isPaused()) {
			static mw::Text text("Paused", tetrisEntry_.getEntry("window font").getFont(30));
			pair.second.setMiddleMessage(text);
		}

		pair.second.draw(wp);
		model = model *  mw::getTranslateMatrix44(pair.second.getWidth() + 5, 0);
		wp->setModel(model);
	}
	*/
	wp->useShader();
	mw::checkGlError();
}

void GameComponent::initGame(const std::vector<PlayerPtr>& players) {
	float width = 5;
	float height = 0;

	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	if (players.size() > 0) {
		graphic_ = Graphic(players.size(), players[0]->getTetrisBoard().getNbrColumns(), players[0]->getTetrisBoard().getNbrRows());
	} else {
		graphic_ = Graphic();
	}

	for (const PlayerPtr& player : players) {
		/*
		auto& pair = graphic_.emplace(player->getId(), Graphic(player, showPoints, spriteZ_,
			spriteS_, spriteJ_, spriteI_,
			spriteL_, spriteT_, spriteO_,
			tetrisEntry_.getEntry("window font").getFont(30)));
		assert(pair.second);
			
		width += pair.first->second.getWidth() + 5;
		height = pair.first->second.getHeight();
		*/
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
	//for (auto& pair : graphic_) {
		//pair.second.setMiddleMessage(text);
	//}
}

namespace {

	inline void addPosition(std::vector<GLfloat>& data, float x, float y) {
		data.push_back(x);
		data.push_back(y);
	}

	inline void addColor(std::vector<GLfloat>& data, mw::Color color) {
		data.push_back(color.red_);
		data.push_back(color.green_);
		data.push_back(color.blue_);
		data.push_back(color.alpha_);
	}

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(std::vector<GLfloat>& data,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		mw::Color color) {

		addPosition(data, x1, y1);
		addColor(data, color);
		
		addPosition(data, x2, y2);
		addColor(data, color);

		addPosition(data, x3, y3);
		addColor(data, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(std::vector<GLfloat>& data,
		float x, float y,
		float w, float h,
		mw::Color color) {
				
		// Left triangle |_
		addTriangle(data, 
			x, y,
			x + w, y,
			x, y + h,
			color);
		//                _
		// Right triangle  |
		addTriangle(data,
			x, y + h,
			x + w, y,
			x + w, y + h,
			color);
	}

}

GameComponent::Graphic::Graphic() {

}

GameComponent::Graphic::Graphic(int nbrPlayers, int nbrColumns, int nbrRows) {
	std::vector<GLfloat> data;
	mw::Color color1, color2, color3;
	color2 = mw::Color(1, 0, 0);

	float blockWidth = 10;
	float boardOffset = 5;
	vertercies_ = 0;
	
	// Add board backgrounds.
	for (int i = 0; i < nbrPlayers; ++i) {
		addSquare(data, 
			blockWidth * i * (nbrColumns + 10), 2 * blockWidth,
			blockWidth * nbrColumns, blockWidth * nbrRows,
			color1);
		vertercies_ += 6;

		for (int row = 0; row < nbrRows; ++row) {
			for (int column = 0; column < nbrColumns; ++column) {
				addSquare(data,
					boardOffset + blockWidth * (column + 0.1f), boardOffset + blockWidth * 0.8f * (row + 0.1f),
					blockWidth*0.8f, blockWidth*0.8f,
					color2);
				vertercies_ += 6;
			}
		}		
	}
	
	vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_STATIC_DRAW);
}

/*
GameComponent::Graphic::Graphic(const PlayerPtr& player, bool showPoints, mw::Sprite spriteZ,
	mw::Sprite spriteS, mw::Sprite spriteJ, mw::Sprite spriteI,
	mw::Sprite spriteL, mw::Sprite spriteT, mw::Sprite spriteO,
	const mw::Font& font) : board_(player->getTetrisBoard()), font_(font) { // info_(font),
	
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
*/

void GameComponent::Graphic::update(const PlayerPtr& player) {
	//PlayerInfo& info = player->getPlayerInfo();
	//info_.update(info.nbrClearedRows_, info.points_, player->getLevel());
	//board_.update();
	//preview_.update(player->getNextBlock(), board_.getPixelPerSquare());
}

float GameComponent::Graphic::getWidth() const {
	//return 5 + board_.getWidth() + 5 + preview_.getWidth() + 5;
	return 300;
}

float GameComponent::Graphic::getHeight() const {
	//return 5 + board_.getHeight() + 5;
	return 300;
}

void GameComponent::Graphic::draw(mw::Shader& shader) {
	if (vbo_.getSize() > 0) {
		vbo_.bindBuffer();

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aPos"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aPos"), 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*) 0);

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aColor"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (GLvoid*) (sizeof(GLfloat) * 2));

		mw::glDrawArrays(GL_TRIANGLES, 0, vertercies_);

		vbo_.unbindBuffer();
		mw::checkGlError();
	}
}

void GameComponent::Graphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameComponent::eventHandler(const PlayerPtr& player, GameEvent gameEvent) {
	//auto it = graphic_.find(player->getId());
	//assert(it != graphic_.end());
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
