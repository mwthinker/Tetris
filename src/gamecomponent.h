#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "tetrisgame.h"
#include "gamesound.h"
#include "graphicboard.h"

#include <gui/component.h>
#include <mw/sound.h>

#include <queue>
#include <thread>
#include <mutex>

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent() : quit_(false), tetrisGame_(this) {
		thread_ = std::thread(std::bind(&GameComponent::run,this));
		networkEvents_.connect(std::bind(&GameComponent::callNetworkEvents, this, std::placeholders::_1));
	}

	~GameComponent() {
		mutex_.lock();
		quit_ = true;
		mutex_.unlock();
		thread_.join();
	}

	void draw(float deltaTime) override {
		std::lock_guard<std::mutex> lock(mutex_);
		glPushMatrix();
		gui::Dimension dim = getSize();
		double width = 0;
		double height = 0;
		int size = players_.size();
		for (int i = 0; i < size; ++i) {
			graphic_[i].update(players_[i]);
			width += graphic_[i].getWidth();
			height = graphic_[i].getHeight();
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
		
		for (int i = 0; i < size; ++i) {
			graphic_[i].board_.draw();
			glTranslated(graphic_[i].board_.getWidth(), 0, 0);
		}
		
		glPopMatrix();

		while (!gameEvents_.empty()) {
			soundEffects(gameEvents_.front());
			gameEvents_.pop();
		}
	}

	void addCallback(mw::Signal<NetworkEventPtr>::Callback callback) {
		std::lock_guard<std::mutex> lock(mutex_);
		networkEvents_.connect(callback);
	}

	void closeGame() {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.closeGame();
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers) {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.createLocalGame(devices, nbrOfComputers);
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int width, int height, int maxLevel) {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.createLocalGame(devices, nbrOfComputers, width, height, maxLevel);
	}

	void createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, int width, int height, int maxLevel) {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.createServerGame(devices, nbrOfComputers, port, width, height, maxLevel);
	}

	void createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, std::string ip, int maxLevel) {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.createClientGame(devices, nbrOfComputers, port, ip, maxLevel);
	}

	void startGame() {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.startGame();
	}

	void restartGame() {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.restartGame();
	}

	void pause() {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.pause();
	}

	void setAis(const Ai& ai1, const Ai& ai2, const Ai& ai3, const Ai& ai4) {
		std::lock_guard<std::mutex> lock(mutex_);
		tetrisGame_.setAis(ai1, ai2, ai3, ai4);
	}

	// Is called by the thread.
	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override {
		std::lock_guard<std::mutex> lock(mutex_);
		gameEvents_.push(gameEvent);
	}

	void initGame(const std::vector<const PlayerPtr>& players) {
		//std::lock_guard<std::mutex> lock(mutex_);
		players_ = players;
		int size = players_.size();
		graphic_ = std::vector<Graphic>(size);

		double width = 0;
		double height = 0;

		for (int i = 0; i < size; ++i) {
			graphic_[i].update(players_[i]);
			width += graphic_[i].board_.getWidth() + 50;
			height = graphic_[i].board_.getHeight();
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

		void update(const PlayerPtr& player) {
			info_.update(player->getNbrClearedRows(), player->getPoints(), player->getLevel(), player->getName());
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

		GraphicPlayerInfo info_;
		GraphicPreviewBlock preview_;
		GraphicBoard board_;
	};

	// Is called by the thread.
	void run() {
		mutex_.lock();
		bool quit = quit_;
		mutex_.unlock();
		while (!quit) {
			tetrisGame_.update(1);
			mutex_.lock();
			quit = quit_;
			mutex_.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	// Makes callbacks thread safe.
	void callNetworkEvents(NetworkEventPtr networkEvent) {
		mutex_.lock();
		networkEvents_(networkEvent);
		mutex_.unlock();
	}

	void soundEffects(GameEvent gameEvent) {
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

	std::queue<GameEvent> gameEvents_;
	mw::Signal<NetworkEventPtr> networkEvents_;
	TetrisGame tetrisGame_;
	std::mutex mutex_;
	std::thread thread_;
	bool quit_;
	std::vector<Graphic> graphic_;
	std::vector<const PlayerPtr> players_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
