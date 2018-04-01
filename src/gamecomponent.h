#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "gamegraphic.h"
#include "boardshader.h"
#include "player.h"

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
	GameComponent(TetrisGame& tetrisGame);
	~GameComponent();
	
	// @gui::Component
	void draw(const gui::Graphic& graphic, double deltaTime) override;

private:
	void initGame(std::vector<PlayerPtr>& player);

	void eventHandler(TetrisGameEvent& tetrisGameEvent);

	// @gui::Component
	// Called when the component is resized or moved.
	void validate() override;

	void handleMiddleText(const PlayerPtr& player, int lastPostion);

	std::map<PlayerPtr, GameGraphic> graphicPlayers_;
	BoardShaderPtr boardShader_;
	
	std::shared_ptr<BoardBatch> staticBoardBatch_;
	std::shared_ptr<BoardBatch> dynamicBoardBatch_;
	
	TetrisGame& tetrisGame_;

	mw::signals::Connection eventConnection_;

	// Fix time step.
	Uint32 timeStep_;
	Uint32 accumulator_;

	// Updated in initGame().
	mw::Matrix44f matrix_;
	bool updateMatrix_;
	
	// Font related.
	mw::Text middleText_;
	float fontSize_;
	float dx_, dy_;
	float scale_;
	float borderSize_;
};

#endif // GAMECOMPONENT_H
