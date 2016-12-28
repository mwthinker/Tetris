#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "gamegraphic.h"
#include "tetrisentry.h"
#include "boardshader.h"
#include "lightningshader.h"

#include <gui/component.h>

#include <mw/vertexbufferobject.h>
#include <mw/shader.h>

#include <mw/signal.h>

#include <map>

class TetrisGame;
class GameData;
class TetrisGameEvent;

class GameComponent : public gui::Component {
public:
	GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry);
	~GameComponent();
	
	// @gui::Component
	void draw(const gui::Graphic& graphic, double deltaTime) override;

	void initGame(std::vector<std::shared_ptr<Player>>& player);

	void countDown(int msCountDown);

	void eventHandler(TetrisGameEvent& tetrisGameEvent);

private:
	void soundEffects(GameEvent gameEvent) const;

	// @gui::Component
	// Called when the component is resized or moved.
	void validate() override;

	std::map<int, GameGraphic> graphicPlayers_;
	BoardShader boardShader_;
	LightningShader lightningShader_;
	
	TetrisEntry tetrisEntry_;
	TetrisGame& tetrisGame_;

	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	mw::signals::Connection eventConnection_;
	Buffer buffer_;

	// Fix time step.
	Uint32 timeStep_;
	Uint32 accumulator_;

	// Updated in initGame().
	Mat44 matrix_;
	bool updateMatrix_;
	int alivePlayers_;

	// Font related.
	mw::Font font_;
	float fontSize_;
	float dx_, dy_;
	float scale_;
	float borderSize_;
};

#endif // GAMECOMPONENT_H
