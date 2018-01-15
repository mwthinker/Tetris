#include "gamecomponent.h"
#include "tetrisgame.h"
#include "gamegraphic.h"
#include "tetrisparameters.h"
#include "tetrisgameevent.h"
#include "tetrisdata.h"

#include <mw/opengl.h>
#include <gui/component.h>

#include <queue>
#include <map>
#include <sstream>
#include <cassert>

namespace {

	std::string gamePosition(int position) {
		std::stringstream stream;
		stream << position;
		switch (position) {
			case 1:
				stream << ":st place!";
				break;
			case 2:
				stream << ":nd place!";
				break;
			case 3:
				stream << ":rd place!";
				break;
			default:
				stream << ":th place!";
				break;
		}
		return stream.str();
	}

}

GameComponent::GameComponent(TetrisGame& tetrisGame)
	: tetrisGame_(tetrisGame),
	updateMatrix_(true) {

	setGrabFocus(true);
	eventConnection_ = tetrisGame_.addGameEventHandler(std::bind(&GameComponent::eventHandler, this, std::placeholders::_1));

	boardShader_ = std::make_shared<BoardShader>("board.ver.glsl", "board.fra.glsl");
	dynamicBoardBatch_ = std::make_shared<BoardBatch>(boardShader_, 10000);
}

GameComponent::~GameComponent() {
	eventConnection_.disconnect();
}

void GameComponent::validate() {
	updateMatrix_ = true;
}

void GameComponent::draw(const gui::Graphic& graphic, double deltaTime) {
	boardShader_->useProgram();
	const gui::Dimension dim = getSize();
	if (updateMatrix_) {
		float width = 0;
		float height = 0;

		for (const auto& pair : graphicPlayers_) {
			const GameGraphic& graphic = pair.second;
			width += graphic.getWidth();
			height = graphic.getHeight();
		}

		// Centers the game and holds the correct proportions.
		// The sides are transparent.
		Mat44 model = getModelMatrix();
		if (width / dim.width_ > height / dim.height_) {
			// Blank sides, up and down.
			scale_ = dim.width_ / width;
			dx_ = 0;
			dy_ = (dim.height_ - scale_ * height) * 0.5f;
		} else {
			// Blank sides, left and right.
			scale_ = dim.height_ / height;
			dx_ = (dim.width_ - scale_ * width) * 0.5f;
			dy_ = 0;
		}

		mw::translate2D(model, dx_, dy_);
		mw::scale2D(model, scale_, scale_);

		boardShader_->setMatrix(graphic.getProjectionMatrix() * model);
		boardShader_->useProgram();
		updateMatrix_ = false;
	}

	if (!graphicPlayers_.empty()) {
		// Draw boards.
		TetrisData::getInstance().bindTextureFromAtlas();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		dynamicBoardBatch_->clear();

		for (auto& pair : graphicPlayers_) {
			GameGraphic& graphic = pair.second;
			graphic.update((float) deltaTime, *dynamicBoardBatch_);
		}
		dynamicBoardBatch_->uploadToGraphicCard();

		staticBoardBatch_->draw();
		dynamicBoardBatch_->draw();
		
		// Draw text.
		for (auto& pair : graphicPlayers_) {
			GameGraphic& graphic = pair.second;
			graphic.drawText(*dynamicBoardBatch_);
		}

		// Draw middle text.
		for (auto& pair : graphicPlayers_) {
			GameGraphic& graphic = pair.second;			
			graphic.drawMiddleText(*dynamicBoardBatch_);
		}
		mw::checkGlError();
	}
}

void GameComponent::initGame(std::vector<PlayerPtr>& players) {
	bool showPoints = false;
	if (players.size() == 1) {
		showPoints = true;
	}

	staticBoardBatch_ = std::make_shared<BoardBatch>(boardShader_);

	graphicPlayers_.clear();

	float w = 0;
	for (auto& player : players) {
		auto& graphic = graphicPlayers_[player];
		graphic.restart(*staticBoardBatch_, *player, w, 0);
		w += graphic.getWidth();
	}
	staticBoardBatch_->uploadToGraphicCard();

	updateMatrix_ = true;
}

void GameComponent::eventHandler(TetrisGameEvent& tetrisEvent) {
	// Handle CountDown event.
	try {
		auto& countDown = dynamic_cast<CountDown&>(tetrisEvent);

		if (countDown.timeLeft_ > 0) {
			middleText_.setText("Start in " + std::to_string(countDown.timeLeft_));
		} else {
			middleText_.setText("");
		}

		// Update the text for the active players.
		for (auto& graphic : graphicPlayers_) {
			if (!graphic.first->getTetrisBoard().isGameOver()) {
				graphic.second.setMiddleMessage(middleText_);
			}
		}
		return;
	} catch (std::bad_cast exp) {}
	
	// Handle GamePause event.
	try {
		auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);

		if (gamePause.printPause_) {
			if (!gamePause.pause_) {
				middleText_.setText("");
			} else {
				middleText_.setText("Paused");
			}

			// Update the text for the active players.
			for (auto& graphic : graphicPlayers_) {
				if (!graphic.first->isGameOver()) {
					graphic.second.setMiddleMessage(middleText_);
				}
			}
		}
		return;
	} catch (std::bad_cast exp) {}

	// Handle InitGame event.
	try {
		middleText_ = mw::Text("", TetrisData::getInstance().getDefaultFont(50), 20);
		auto& initGameVar = dynamic_cast<InitGame&>(tetrisEvent);
		initGame(initGameVar.players_);
		
		for (const PlayerPtr& player : initGameVar.players_) {
			if (player->isGameOver()) {
				handleMiddleText(player, player->getLastPositon());
			}
		}

		return;
	} catch (std::bad_cast exp) {}

	// Handle LevelChange event.
	try {
		auto& levelChange = dynamic_cast<LevelChange&>(tetrisEvent);
		GameGraphic& gg = graphicPlayers_[levelChange.player_];
		gg.update(levelChange.player_->getClearedRows(), levelChange.player_->getPoints(), levelChange.newLevel_);
		return;
	} catch (std::bad_cast exp) {}

	// Handle PointsChange event.
	try {
		auto& pointsChange = dynamic_cast<PointsChange&>(tetrisEvent);
		GameGraphic& gg = graphicPlayers_[pointsChange.player_];
		gg.update(pointsChange.player_->getClearedRows(), pointsChange.player_->getPoints(), pointsChange.player_->getLevel());
		return;
	} catch (std::bad_cast exp) {}

	// Handle GameOver event.
	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);
		handleMiddleText(gameOver.player_, gameOver.position_);

		return;
	} catch (std::bad_cast exp) {}
}

void GameComponent::handleMiddleText(const PlayerPtr& player, int lastPostion) {
	mw::Text middleText("", TetrisData::getInstance().getDefaultFont(50), 20);

	// Test if the player is a local player, exception otherwise.
	if (tetrisGame_.getNbrOfPlayers() == 1) {
		middleText.setText("Game over");
	} else {
		middleText.setText(gamePosition(lastPostion));
	}

	graphicPlayers_[player].setMiddleMessage(middleText);
}
