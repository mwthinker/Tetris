#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"

#include <gui/component.h>
#include <mw/sound.h>

#include <queue>
#include <map>

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent() : tetrisGame_(this) {
	}

	~GameComponent() {
	}

	void draw(Uint32 deltaTime) override {
		tetrisGame_.update(deltaTime*1000);

		glPushMatrix();
		gui::Dimension dim = getSize();
		double width = 0;
		double height = 0;
		for (auto pair : graphic_) {
			width += pair.second.getWidth();
			height = pair.second.getHeight();
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
		
		for (auto pair : graphic_) {
			pair.second.draw();
			glTranslated(pair.second.getWidth(), 0, 0);
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

		double width = 0;
		double height = 0;

		for (const PlayerPtr& player : players) {
			Graphic& graphic = graphic_[player->getId()];
			graphic = Graphic(player);
			width += graphic.getWidth() + 50;
			height = graphic.getHeight();
		}

		setPreferredSize((float) width, (float) height);
	}

private:
	struct Graphic {
		Graphic() {
		}

		Graphic(const PlayerPtr& player) {
			update(player);
		}

		/*
		Graphic(const Graphic& player) : info_(player.info_), preview_(player.preview_), board_(player.board_) {
		}

		Graphic& operator=(const Graphic& player) {
			//info_ = player.info_;
			preview_ = player.preview_;
			board_ = player.board_;
			return *this;
		}
		*/

		void update(const PlayerPtr& player) {
			//info_.update(player->getNbrClearedRows(), player->getPoints(), player->getLevel(), player->getName());
			preview_.update(player->getNextBlock());
			board_.update(player->getTetrisBoard());
		}

		inline double getWidth() const {
			return board_.getWidth();
		}

		inline double getHeight() const {
			return board_.getHeight();
		}

		inline void draw() {
			board_.draw();
		}

		//GraphicPlayerInfo info_;
		GraphicPreviewBlock preview_;
		GraphicBoard board_;
	};

	// Is called by the thread.
	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override {
		graphic_[player->getId()].update(player);
		soundEffects(gameEvent);
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

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
