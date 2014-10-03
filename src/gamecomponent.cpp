#include "gamecomponent.h"
#include "tetrisgame.h"
#include "graphicboard.h"
#include "tetrisparameters.h"
#include "tetrisentry.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

namespace {

	template <class A>
	void drawLineBorder(const A& a) {
		/*
		glBegin(GL_LINE_LOOP);
		glVertex2d(0, 0);
		glVertex2d(a.getWidth(), 0);
		glVertex2d(a.getWidth(), a.getHeight());
		glVertex2d(0, a.getHeight());
		glEnd();
		*/
	}

}

GameComponent::GameComponent(TetrisGame& tetrisGame, TetrisEntry tetrisEntry) : tetrisGame_(tetrisGame), tetrisEntry_(tetrisEntry), alivePlayers_(0) {
	setGrabFocus(true);
	tetrisGame_.setGameHandler(this);

	soundBlockCollision_ = tetrisEntry_.getEntry("window sounds blockCollision").getSound();
	soundRowRemoved_ = tetrisEntry_.getEntry("window sounds rowRemoved").getSound();
	soundTetris_ = tetrisEntry_.getEntry("window sounds tetris").getSound();
	
	// Must bind attributes before linking.
	boardShader_.bindAttribute("aPos");
	boardShader_.bindAttribute("aTex");
	boardShader_.bindAttribute("aIsTex");
	boardShader_.bindAttribute("aColor");
	boardShader_.loadAndLinkFromFile("board.ver.glsl", "board.fra.glsl");
}

void GameComponent::draw(Uint32 deltaTime) {
	gui::Dimension dim = getSize();

	float width = graphic_.getWidth();
	float height = graphic_.getHeight();

	// Centers the game and holds the correct proportions.
	// The sides is transparent.
	mw::Matrix44 model;
	if (width / dim.width_ > height / dim.height_) {
		// Blank sides, up and down.
		float scale = dim.width_ / width;
		model = mw::getTranslateMatrix44(5, (dim.height_ - scale * height) * 0.5f + 5) * mw::getScaleMatrix44(scale, scale);
	} else {
		// Blank sides, left and right.
		float scale = dim.height_ / height;
		model = mw::getTranslateMatrix44(5 + (dim.width_ - scale * width) * 0.5f, 5) * mw::getScaleMatrix44(scale, scale);
	}

	auto wp = getWindowMatrixPtr();
	mw::Matrix44 m = wp->getProjection() * wp->getModel() * model;

	boardShader_.glUseProgram();
	mw::glUniformMatrix4fv(boardShader_.getUniformLocation("uMat"), 1, false, m.data());
	mw::glEnable(GL_BLEND);
	mw::glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	mw::glActiveTexture(GL_TEXTURE1);

	graphic_.draw(boardShader_);

	mw::glActiveTexture(GL_TEXTURE0);
	mw::glDisable(GL_BLEND);

	/*
	for (auto& pair : graphic_) {
		if (tetrisGame_.isPaused()) {
			static mw::Text text("Paused", tetrisEntry_.getEntry("window font").getFont(30));
			pair.second.setMiddleMessage(text);
		}

		pair.second.draw(wp);
		model = model *  mw::getTranslateMatrix44(pair.second.getWidth() + 5, 0);
		wp->setModel(model);
	}
	*/
	wp->useShader();
	mw::checkGlError();
}

void GameComponent::initGame(const std::vector<PlayerPtr>& players) {
	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	if (players.size() > 0) {
		graphic_ = Graphic(tetrisEntry_.getEntry("window tetrisBoard"), players[0]->getTetrisBoard());
	} else {
		graphic_ = Graphic();
	}
	
	alivePlayers_ = players.size();
	setPreferredSize(graphic_.getWidth(), graphic_.getHeight());
}

void GameComponent::countDown(int msCountDown) {
	mw::Text text("", tetrisEntry_.getEntry("window font").getFont(30));
	if (msCountDown > 0) {
		std::stringstream stream;
		stream << "Start in " << (int) (msCountDown / 1000) + 1;
		text.setText(stream.str());
	}
}

namespace {

	inline void addVertex(std::vector<GLfloat>& data,
		float x, float y, 
		float xTex, float yTex,
		bool isTex,
		mw::Color color) {
		data.push_back(x);
		data.push_back(y);

		data.push_back(xTex);
		data.push_back(yTex);

		data.push_back(isTex);

		data.push_back(color.red_);
		data.push_back(color.green_);
		data.push_back(color.blue_);
		data.push_back(color.alpha_);
	}	

	// Add a triangle, GL_TRIANGLES, i.e. 3 vertices.
	inline void addTriangle(std::vector<GLfloat>& data,
		float x1, float y1,
		float x2, float y2,
		float x3, float y3,
		float xTex1, float yTex1,
		float xTex2, float yTex2,
		float xTex3, float yTex3,
		bool isTex,
		mw::Color color) {

		addVertex(data, x1, y1, xTex1, yTex1, isTex, color);
		addVertex(data, x2, y2, xTex1, yTex1, isTex, color);
		addVertex(data, x3, y3, xTex1, yTex1, isTex, color);
	}

	// Add two triangles, GL_TRIANGLES, i.e. 6 vertices.
	inline void addSquare(std::vector<GLfloat>& data,
		float x, float y,
		float w, float h,
		mw::Color color) {
				
		// Left triangle |_
		addTriangle(data, 
			x, y,
			x + w, y,
			x, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
		//                _
		// Right triangle  |
		addTriangle(data,
			x, y + h,
			x + w, y,
			x + w, y + h,
			0, 0,
			0, 0,
			0, 0,
			false,
			color);
	}

	inline void addSquare(std::vector<GLfloat>& data,
		float x, float y,
		float w, float h,		
		mw::Sprite& sprite) {
		int textureW = sprite.getTexture().getWidth();
		int textureH = sprite.getTexture().getHeight();

		// Left triangle |_
		addTriangle(data,
			x, y,
			x + w, y,
			x, y + h,
			sprite.getX() / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			mw::Color(1, 1, 1));
		//                _
		// Right triangle  |
		addTriangle(data,
			x, y + h,
			x + w, y,
			x + w, y + h,
			sprite.getX() / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, sprite.getY() / textureH,
			(sprite.getX() + sprite.getWidth()) / textureW, (sprite.getY() + sprite.getHeight()) / textureH,
			true,
			mw::Color(1, 1, 1));
	}

	mw::Sprite getBoardSprite(mw::Texture texture, TetrisEntry spriteEntry) {
		float x = spriteEntry.getChildEntry("x").getFloat();
		float y= spriteEntry.getChildEntry("y").getFloat();
		float w = spriteEntry.getChildEntry("w").getFloat();
		float h = spriteEntry.getChildEntry("h").getFloat();
		return mw::Sprite(texture, x, y, w, h);
	}

}

GameComponent::Graphic::Graphic() {

}

void GameComponent::Graphic::fillBoard(std::vector<GLfloat>& data, int player) {

}

GameComponent::Graphic::Graphic(TetrisEntry boardEntry, const RawTetrisBoard& tetrisBoard) {
	mw::Color color1 = boardEntry.getChildEntry("outerSquareColor").getColor();
	mw::Color color2 = boardEntry.getChildEntry("innerSquareColor").getColor();
	mw::Color color3 = boardEntry.getChildEntry("startAreaColor").getColor();
	mw::Color color4 = boardEntry.getChildEntry("playerAreaColor").getColor();

	mw::Texture texture = boardEntry.getChildEntry("texture").getTexture();
	spriteZ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareZ"));
	spriteS_ = getBoardSprite(texture, boardEntry.getChildEntry("squareS"));
	spriteJ_ = getBoardSprite(texture, boardEntry.getChildEntry("squareJ"));
	spriteI_ = getBoardSprite(texture, boardEntry.getChildEntry("squareI"));
	spriteL_ = getBoardSprite(texture, boardEntry.getChildEntry("squareL"));
	spriteT_ = getBoardSprite(texture, boardEntry.getChildEntry("squareT"));
	spriteO_ = getBoardSprite(texture, boardEntry.getChildEntry("squareO"));

	int rows = tetrisBoard.getNbrRows();
	int columns = tetrisBoard.getNbrColumns();

	const float lowX = 20;
	const float lowY = 20;

	const float squareSize = 10;
	const float sizeBetweenPlayers = 10;
	const float sizeBoard = squareSize * columns;
	const float sizeInfo = 100;
	const float sizePlayer = lowX + sizeBoard + sizeInfo;
	vertercies_ = 0;
	
	std::vector<GLfloat> data;
	// Draw the player area.
	float x = (sizeBoard + sizeBetweenPlayers) * 0;
	float y = lowY * 0.5f;
	addSquare(data,
		x, y,
		sizeBoard + sizeInfo + 2 * lowX, squareSize * (rows + 2) + lowY,
		color4);
	vertercies_ += 6;

	// Draw the outer square.
	x = lowX + 0 * (squareSize * columns + sizeInfo + sizeBetweenPlayers);
	y = lowY;
	addSquare(data, 
		x, y,
		squareSize * columns, squareSize * (rows + 2),
		color1);
	vertercies_ += 6;

	indexBoard_ = data.size();

	// Draw the inner squares.
	for (int row = 0; row < rows + 2; ++row) {
		for (int column = 0; column < columns; ++column) {
			x = lowX + squareSize * column + (squareSize * columns + sizeInfo) * 0 + squareSize * 0.1f;
			y = lowY + squareSize * row + squareSize * 0.1f;
			addSquare(data,
				x, y,
				squareSize * 0.8f, squareSize * 0.8f,
				color2);
			vertercies_ += 6;
		}
	}

	// Draw the block start area.
	x = lowX + (sizeBoard + sizeInfo + sizeBetweenPlayers) * 0;
	y = lowY + squareSize * rows;
	addSquare(data,
		x, y,
		squareSize * columns, squareSize * 2,
		color3);
	vertercies_ += 6;

	// Draw the current block.
	x = lowX;
	y = lowY;
	Block block = tetrisBoard.getBlock();
	mw::Sprite sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		addSquare(data,
			x + sq.column_ * squareSize, y + sq.row_ * squareSize,
			squareSize, squareSize,
			sprite);
		vertercies_ += 6;
	}
	vertercies_ += 6;

	// Draw the next block.
	x = lowX + sizeBoard;
	y = lowY;
	block = Block(tetrisBoard.getNextBlockType(), 0, 0);
	sprite = getSprite(block.blockType());
	for (const Square& sq : block) {
		addSquare(data,
			x + sq.column_ * squareSize, y + sq.row_ * squareSize,
			squareSize, squareSize,
			sprite);
		vertercies_ += 6;
	}
	vertercies_ += 6;	
	
	width_ = squareSize * columns + (sizeBoard + sizeBetweenPlayers) * (1 - 1) + lowX * 2;
	height_ = squareSize * (rows + 2) + lowY * 2;

	vbo_.bindBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * data.size(), data.data(), GL_DYNAMIC_DRAW);
}

void GameComponent::Graphic::update(const PlayerPtr& player) {
}

void GameComponent::Graphic::draw(mw::Shader& shader) {
	if (vbo_.getSize() > 0) {
		vbo_.bindBuffer();
		spriteI_.bindTexture(); // All sprites uses the same texture.

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aPos"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aPos"), 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) 0);
		
		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aTex"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aTex"), 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 2));
		
		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aIsTex"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aIsTex"), 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 4));

		mw::glEnableVertexAttribArray(shader.getAttributeLocation("aColor"));
		mw::glVertexAttribPointer(shader.getAttributeLocation("aColor"), 4, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 9, (GLvoid*) (sizeof(GLfloat) * 5));

		mw::glDrawArrays(GL_TRIANGLES, 0, vertercies_);

		vbo_.unbindBuffer();
		mw::checkGlError();
	}
}

mw::Sprite GameComponent::Graphic::getSprite(BlockType blockType) const {
	switch (blockType) {
		case BlockType::I:
			return spriteI_;
		case BlockType::J:
			return spriteJ_;
		case BlockType::L:
			return spriteL_;
		case BlockType::O:
			return spriteO_;
		case BlockType::S:
			return spriteS_;
		case BlockType::T:
			return spriteT_;
		case BlockType::Z:
			return spriteZ_;
	}
	return mw::Sprite();
}

void GameComponent::Graphic::setMiddleMessage(const mw::Text& text) {
	middleMessage_ = text;
}

void GameComponent::eventHandler(const PlayerPtr& player, GameEvent gameEvent) {
	soundEffects(gameEvent);
	switch (gameEvent) {
		case GameEvent::GAME_OVER:
			if (tetrisGame_.getNbrOfPlayers() == 1) {
				mw::Text text("Game Over", tetrisEntry_.getEntry("window font").getFont(30));
				//it->second.setMiddleMessage(text);
			} else {
				std::stringstream stream;
				stream << alivePlayers_;
				if (alivePlayers_ == 1) {
					stream << ":st place!";
				} else if (alivePlayers_ == 2) {
					stream << ":nd place!";
				} else if (alivePlayers_ == 3) {
					stream << ":rd place!";
				} else {
					stream << ":th place!";
				}
				--alivePlayers_;
				//mw::Text text(stream.str(), tetrisEntry_.getEntry("window font").getFont(30));
				//it->second.setMiddleMessage(text);
			}
			break;
	}
}

void GameComponent::soundEffects(GameEvent gameEvent) const {
	mw::Sound sound;
	switch (gameEvent) {
		case GameEvent::GRAVITY_MOVES_BLOCK:
			break;
		case GameEvent::BLOCK_COLLISION:
			sound = soundBlockCollision_;
			break;
		case GameEvent::PLAYER_ROTATES_BLOCK:
			break;
		case GameEvent::ONE_ROW_REMOVED:
			// Fall through
		case GameEvent::TWO_ROW_REMOVED:
			// Fall through
		case GameEvent::THREE_ROW_REMOVED:
			sound = soundRowRemoved_;
			break;
		case GameEvent::FOUR_ROW_REMOVED:
			sound = soundTetris_;
			break;
		case GameEvent::GAME_OVER:
			break;
	}
	sound.play();
}
