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
		tetrisGame_.draw();
	}

private:
	TetrisGame& tetrisGame_;
};

#endif // GAMECOMPONENT_H
