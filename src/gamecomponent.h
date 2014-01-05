#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include <gui/component.h>

#include "tetrisgame.h"

class GameComponent : public gui::Component {
public:
	GameComponent() {
		setPreferredSize((float) tetrisGame_.getWidth(), (float) tetrisGame_.getHeight());
	}

	void draw(float deltaTime) override {
		glPushMatrix();
		gui::Dimension dim = getSize();

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
		glPopMatrix();

		tetrisGame_.update(deltaTime * 1000);
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
		tetrisGame_.createLocalGame(devices, 0, width, height, maxLevel);
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

private:
	TetrisGame tetrisGame_;
};

#endif // GAMECOMPONENT_H
