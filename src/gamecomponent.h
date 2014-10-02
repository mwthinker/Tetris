#ifndef GAMECOMPONENT_H
#define GAMECOMPONENT_H

#include "graphicboard.h"
#include "gamehandler.h"
#include "tetrisentry.h"

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
	class Graphic {
	public:
		Graphic();

		Graphic(TetrisEntry boardEntry, int nbrPlayers, int nbrColumns, int nbrRows);

		void update(const PlayerPtr& player);

		inline float getWidth() const {
			return width_;
		}

		inline float getHeight() const {
			return height_;
		}

		void draw(mw::Shader& shader);

		void setMiddleMessage(const mw::Text& text);		

	private:
		void fillBoard(std::vector<GLfloat>& data, int player);
		std::vector<int> indexes_;

		float squareSize_;
		
		mw::Text name_;
		mw::Text middleMessage_;
		mw::Font font_;
		mw::VertexBufferObject vbo_;
		int vertercies_;
		float width_, height_;
		mw::Sprite spriteI_, spriteJ_, spriteL_, spriteO_, spriteS_, spriteT_, spriteZ_;
	};

	void eventHandler(const PlayerPtr& player, GameEvent gameEvent) override;

	void soundEffects(GameEvent gameEvent) const;

	TetrisGame& tetrisGame_;
	int alivePlayers_;

	TetrisEntry tetrisEntry_;

	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;

	mw::Shader boardShader_;
	Graphic graphic_;

	// Fix timestep.
	Uint32 timeStep_;
	Uint32 accumulator_;
};

#endif // GAMECOMPONENT_H
