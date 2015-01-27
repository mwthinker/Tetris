#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "gamegraphic.h"
#include "gamehandler.h"
#include "tetrisentry.h"
#include "boardshader.h"

#include <gui/component.h>

#include <mw/vertexbufferobject.h>
#include <mw/shader.h>

#include <map>

class TetrisGame;
class GameData;

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry);
	
	void draw(Uint32 deltaTime) override;

	void initGame(std::vector<PlayerPtr>& players) override;

	void countDown(int msCountDown) override;

private:
	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override;

	void soundEffects(GameEvent gameEvent) const;

	// @gui::Component
	// Called when the component is resized or moved.
	void validate() override;

	std::map<int, GameGraphic> graphicPlayers_;
	BoardShader boardShader_;
	TetrisEntry tetrisEntry_;
	TetrisGame& tetrisGame_;
	
	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;

	// Updated in init.
	mw::Matrix44 matrix_;
	bool updateMatrix_;
	int alivePlayers_;
	
	// Font related.
	mw::Font font_;
	float fontSize_;
	float dx_, dy_;
	float scale_;
};

#endif // GAMECOMPONENT_H
