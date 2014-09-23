#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "graphicboard.h"
#include "gamehandler.h"
#include "tetrisentry.h"

#include <gui/component.h>

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
	class Graphic {
	public:
		Graphic(const PlayerPtr& player, bool showPoints, mw::Sprite spriteZ, 
			mw::Sprite spriteS, mw::Sprite spriteJ, mw::Sprite spriteI, 
			mw::Sprite spriteL, mw::Sprite spriteT, mw::Sprite spriteO,
			const mw::Font& font);

		void update(const PlayerPtr& player);

		float getWidth() const;

		float getHeight() const;

		void draw(gui::WindowMatrixPtr wp);

		void setMiddleMessage(const mw::Text& text);		

	private:
		GraphicBoard board_;
		GraphicPlayerInfo info_;
		GraphicPreviewBlock preview_;
		mw::Text name_;
		mw::Text middleMessage_;
		mw::Font font_;
	};

	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override;

	void soundEffects(GameEvent gameEvent) const;

	TetrisGame& tetrisGame_;
	std::map<int, Graphic> graphic_;
	int alivePlayers_;

	TetrisEntry tetrisEntry_;
	mw::Sprite spriteZ_, spriteS_, spriteJ_, spriteI_, spriteL_, spriteT_, spriteO_;

	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
