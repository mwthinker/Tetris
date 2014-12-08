#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "gamegraphic.h"
#include "gamehandler.h"
#include "tetrisentry.h"
#include "boardshader.h"

#include <gui/component.h>

#include <mw/vertexbufferobject.h>
#include <mw/shader.h>

#include <queue>
#include <map>

class TetrisGame;
class GameData;

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry);
	
	void draw(Uint32 deltaTime) override;

	void initGame(const std::vector<PlayerPtr>& players) override;

	void countDown(int msCountDown) override;

private:
	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override;

	void soundEffects(GameEvent gameEvent) const;

	// @gui::Component
	// Called when the component is resized.
	void validate() override;

	mw::Font font_;
	float fontSize_;
	float dx_, dy_;
	float scale_;

	std::map<int, GameGraphic> graphicPlayers_;
	TetrisGame& tetrisGame_;
	int alivePlayers_;

	TetrisEntry tetrisEntry_;

	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	BoardShader boardShader_;	

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;

	bool updateMatrix_;
	mw::Matrix44 matrix_;
};

#endif // GAMECOMPONENT_H
