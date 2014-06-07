#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "graphicboard.h"
#include "gamehandler.h"

#include <gui/component.h>

#include <queue>
#include <map>

class TetrisGame;
class GameData;

class GameComponent : public gui::Component, public GameHandler {
public:
	GameComponent(TetrisGame& tetrisGame, GameData& gameData);

	inline ~GameComponent() {
	}

	void draw(Uint32 deltaTime) override;

	void initGame(const std::vector<PlayerPtr>& players) override;

	void countDown(int msCountDown) override;

private:
	class Graphic {
	public:
		Graphic();

		Graphic(const PlayerPtr& player, bool showPoints, mw::Sprite spriteZ, 
			mw::Sprite spriteS, mw::Sprite spriteJ, mw::Sprite spriteI, 
			mw::Sprite spriteL, mw::Sprite spriteT, mw::Sprite spriteO);

		void update(const PlayerPtr& player);

		inline float getWidth() const;

		inline float getHeight() const;

		inline void draw();

		void setMiddleMessage(const mw::Text& text);		

	private:
		GraphicBoard board_;
		GraphicPlayerInfo info_;
		GraphicPreviewBlock preview_;
		mw::Text name_;
		mw::Text middleMessage_;
	};

	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override;

	void soundEffects(GameEvent gameEvent) const;

	TetrisGame& tetrisGame_;
	std::map<int, Graphic> graphic_;
	int alivePlayers_;

	GameData& gameData_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
