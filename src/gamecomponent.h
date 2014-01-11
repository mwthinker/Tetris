#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"

#include <gui/component.h>
#include <mw/sound.h>

#include <queue>
#include <map>

template <class A>
void drawLineBorder(const A& a) {
	glBegin(GL_LINE_LOOP);
	glVertex2d(0, 0);
	glVertex2d(a.getWidth(), 0);
	glVertex2d(a.getWidth(), a.getHeight());
	glVertex2d(0, a.getHeight());
	glEnd();
}

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent() : tetrisGame_(this), alivePlayers_(0) {
		setAlwaysFocus(true);
	}

	~GameComponent() {
	}

	void draw(Uint32 deltaTime) override {
		tetrisGame_.update(deltaTime);

		glPushMatrix();
		gui::Dimension dim = getSize();
		double width = 5;
		double height = 5;
		for (const auto& pair : graphic_) {
			width += pair.second.getWidth() + 5;
			height = 5 + pair.second.getHeight() + 5;
		}
		
		// Centers the game and holds the correct proportions.
		// The sides is transparent.
		if (width / dim.width_ > height / dim.height_) {
			// Blank sides, up and down.
			double scale = (double) dim.width_ / width;
			glTranslated(0, (dim.height_ - scale * height) * 0.5, 0);
			glScaled(scale, scale, 1);
		} else {
			// Blank sides, left and right.
			double scale = (double) dim.height_ / height;
			glTranslated((dim.width_ - scale * width) * 0.5, 0, 0);
			glScaled(scale, scale, 1);
		}
		
		glTranslated(5, 5, 0);
		for (auto& pair : graphic_) {
			if (tetrisGame_.isPaused()) {
				static mw::Text text("Paused", fontDefault30);
				pair.second.setMiddleMessage(text);
			}

			pair.second.draw();
			glTranslated(pair.second.getWidth() + 5, 0, 0);
		}

		glPopMatrix();
	}

	void addCallback(mw::Signal<NetworkEventPtr>::Callback callback) {
		tetrisGame_.addCallback(callback);
	}

	void closeGame() {
		tetrisGame_.closeGame();
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers) {
		tetrisGame_.createLocalGame(devices, nbrOfComputers);
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int width, int height, int maxLevel) {
		tetrisGame_.createLocalGame(devices, nbrOfComputers, width, height, maxLevel);
	}

	void createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, int width, int height, int maxLevel) {
		tetrisGame_.createServerGame(devices, nbrOfComputers, port, width, height, maxLevel);
	}

	void createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, std::string ip, int maxLevel) {
		tetrisGame_.createClientGame(devices, nbrOfComputers, port, ip, maxLevel);
	}

	void startGame() {
		tetrisGame_.startGame();
	}

	void restartGame() {
		tetrisGame_.restartGame();
	}

	void pause() {
		tetrisGame_.pause();
	}

	void setAis(const Ai& ai1, const Ai& ai2, const Ai& ai3, const Ai& ai4) {
		tetrisGame_.setAis(ai1, ai2, ai3, ai4);
	}	

	void initGame(const std::vector<const PlayerPtr>& players) override{
		graphic_.clear();

		double width = 5;
		double height = 0;

		for (const PlayerPtr& player : players) {
			Graphic& graphic = graphic_[player->getId()];
			graphic = Graphic(player);
			width += graphic.getWidth() + 5;
			height = graphic.getHeight();
		}
		alivePlayers_ = players.size();
		setPreferredSize((float) width, (float) height);
	}

	void countDown(int msCountDown) override {
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

private:
	class Graphic {
	public:
		Graphic() {
			name_ = mw::Text("", fontDefault30, 20);
		}

		Graphic(const PlayerPtr& player) {
			update(player);
			name_ = mw::Text(player->getName(), fontDefault30, 20);
		}

		void update(const PlayerPtr& player) {
			info_.update(player->getNbrClearedRows(), player->getPoints(), player->getLevel());
			board_.update(player->getTetrisBoard());
			preview_.update(player->getNextBlock(), board_.getPixelPerSquare());
		}

		inline double getWidth() const {
			return 5 + board_.getWidth() + 5 + preview_.getWidth() + 5;
		}

		inline double getHeight() const {
			return 5 + board_.getHeight() + 5;
		}

		inline void draw() {
			glPushMatrix();
			
			glTranslated(5, 5, 0);
			board_.draw();
			mw::Color color = mw::Color(237 / 256.0, 78 / 256.0, 8 / 256.0);
			color.glColor3d();
			drawLineBorder(board_);

			glPushMatrix();
			glTranslated(board_.getWidth() + 5, board_.getHeight() - name_.getHeight(), 0);
			glColor3d(1, 1, 1);
			name_.draw();
			glTranslated(0, -5 -preview_.getHeight(), 0);
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

		void setMiddleMessage(const mw::Text& text) {
			middleMessage_ = text;
		}

		GraphicBoard board_;

	private:
		GraphicPlayerInfo info_;
		GraphicPreviewBlock preview_;		
		mw::Text name_;
		mw::Text middleMessage_;
	};

	// Is called by the thread.
	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override {
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

	void soundEffects(GameEvent gameEvent) const {
		mw::Sound sound;
		switch (gameEvent) {
			case GameEvent::GRAVITY_MOVES_BLOCK:
				break;
			case GameEvent::BLOCK_COLLISION:
				sound = soundBlockCollision;
				break;
			case GameEvent::PLAYER_ROTATES_BLOCK:
				break;
			case GameEvent::ONE_ROW_REMOVED:
				// Fall through
			case GameEvent::TWO_ROW_REMOVED:
				// Fall through
			case GameEvent::THREE_ROW_REMOVED:
				sound = soundRowRemoved;
				break;
			case GameEvent::FOUR_ROW_REMOVED:
				sound = soundTetris;
				break;
			case GameEvent::GAME_OVER:
				break;
			default:
				break;
		}
		sound.play();
	}
	
	TetrisGame tetrisGame_;
	std::map<int, Graphic> graphic_;
	int alivePlayers_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
