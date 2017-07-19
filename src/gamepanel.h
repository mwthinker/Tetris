#ifndef GAMEPANEL_H
#define GAMEPANEL_H

#include "gamegraphic.h"
#include "boardshader.h"
#include "lightningshader.h"
#include "gameboardcomponent.h"

#include <gui/component.h>
#include <gui/combobox.h>
#include <gui/panel.h>
#include "guiclasses.h"

#include <mw/vertexbufferobject.h>
#include <mw/shader.h>

#include <mw/signal.h>

#include <map>

class TetrisGame;
class GameData;
class TetrisGameEvent;

class GamePanel : public TransparentPanel {
public:
	GamePanel(TetrisGame& tetrisGame);
	~GamePanel();

	void initGame(std::vector<PlayerPtr>& player);

	void countDown(int msCountDown);

	void eventHandler(TetrisGameEvent& tetrisGameEvent);

protected:
	// @gui::Component
	void draw(const gui::Graphic& graphic, double deltaTime) override;

	// @gui::Component
	void init() override;

private:
	void soundEffects(GameEvent gameEvent) const;

	// @gui::Component
	// Called when the component is resized or moved.
	void validate() override;

	std::shared_ptr<ComboBox> comboBox_;
	std::map<int, GameGraphic> graphicPlayers_;
	BoardShader boardShader_;
	LightningShader lightningShader_;
	
	TetrisGame& tetrisGame_;

	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	mw::signals::Connection eventConnection_;
	mw::Buffer buffer_;

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

#endif // GAMEPANEL_H
