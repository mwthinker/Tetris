#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "tetrisgame.h"
#include "gamesound.h"

#include <gui/component.h>
#include <mw/sound.h>

#include <thread>
#include <mutex>

class GameComponent : public gui::Component {
public:
	GameComponent() : quit_(false) {
		//setPreferredSize((float) tetrisGame_.getWidth(), (float) tetrisGame_.getHeight());
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
		glPushMatrix();
		gui::Dimension dim = getSize();
		mutex_.lock();
		/*
		// Centers the game and holds the correct proportions. The sides is transparent.
		if (tetrisGame_.getWidth() / dim.width_ > tetrisGame_.getHeight() / dim.height_) {
			// Black sides, up and down.
			double scale = (double) dim.width_ / tetrisGame_.getWidth();
			glTranslated(0, (dim.height_ - scale * tetrisGame_.getHeight()) * 0.5, 0);
			glScaled(scale, scale, 1);
		} else {
			// Black sides, left and right.
			double scale = (double) dim.height_ / tetrisGame_.getHeight();
			glTranslated((dim.width_ - scale*tetrisGame_.getWidth()) * 0.5, 0, 0);
			glScaled(scale,scale,1);
		}
		tetrisGame_.draw();
		*/
		glPopMatrix();
		mutex_.unlock();
	}

	void addCallback(mw::Signal<NetworkEventPtr>::Callback callback) {
		mutex_.lock();
		networkEvents_.connect(callback);
		mutex_.unlock();
	}

	void closeGame() {
		mutex_.lock();
		tetrisGame_.closeGame();
		mutex_.unlock();
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers) {
		mutex_.lock();
		tetrisGame_.createLocalGame(devices, nbrOfComputers);
		mutex_.unlock();
	}

	void createLocalGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int width, int height, int maxLevel) {
		mutex_.lock();
		tetrisGame_.createLocalGame(devices, nbrOfComputers, width, height, maxLevel);
		mutex_.unlock();
	}

	void createServerGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, int width, int height, int maxLevel) {
		mutex_.lock();
		tetrisGame_.createServerGame(devices, nbrOfComputers, port, width, height, maxLevel);
		mutex_.unlock();
	}

	void createClientGame(const std::vector<DevicePtr>& devices, int nbrOfComputers, int port, std::string ip, int maxLevel) {
		mutex_.lock();
		tetrisGame_.createClientGame(devices, nbrOfComputers, port, ip, maxLevel);
		mutex_.unlock();
	}

	void startGame() {
		mutex_.lock();
		tetrisGame_.startGame();
		mutex_.unlock();
	}

	void restartGame() {
		mutex_.lock();
		tetrisGame_.restartGame();
		mutex_.unlock();
	}

	void pause() {
		mutex_.lock();
		tetrisGame_.pause();
		mutex_.unlock();
	}

	void setAis(const Ai& ai1, const Ai& ai2, const Ai& ai3, const Ai& ai4) {
		mutex_.lock();
		tetrisGame_.setAis(ai1, ai2, ai3, ai4);
		mutex_.unlock();
	}

private:
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

	mw::Signal<NetworkEventPtr> networkEvents_;
	TetrisGame tetrisGame_;
	std::mutex mutex_;
	std::thread thread_;
	bool quit_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
