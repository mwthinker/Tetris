#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include <gui/component.h>

#include "tetrisgame.h"

class GameComponent : public gui::Component {
public:
	GameComponent(TetrisGame& tetrisGame) : tetrisGame_(tetrisGame) {
		setPreferredSize((float) tetrisGame.getWidth(), (float) tetrisGame.getHeight());
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

private:
	TetrisGame& tetrisGame_;
};

#endif // GAMECOMPONENT_H