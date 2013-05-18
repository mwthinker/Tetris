#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "guiwindow.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "devicejoystick.h"
#include "devicekeyboard.h"

#include <mw/gamewindow.h>
#include <mw/sprite.h>
#include <mw/color.h>

#include <iostream>
#include <fstream>

class TetrisWindow : public GuiWindow {
public:
	TetrisWindow() {
		tetrisGame_.addCallback([&](NetworkEventPtr nEvent) {
			handleConnectionEvent(nEvent);
		});

		// Initializes default keybord devices for two players.
		DevicePtr device1(new InputKeyboard(SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP));
		devices_.push_back(device1);
		DevicePtr device2(new InputKeyboard(SDLK_s, SDLK_a, SDLK_d, SDLK_w));
		devices_.push_back(device2);

		// Init joysticks!
		auto joystics = mw::Joystick::getJoystics();
		for(mw::JoystickPtr& joystick : joystics) {
			std::cout << joystick->getName() << std::endl;
			DevicePtr device(new InputJoystick(joystick,0,1));
			devices_.push_back(device);
		}

        std::vector<DevicePtr> onePlayer;
        onePlayer.push_back(device1);
		tetrisGame_.createLocalGame(onePlayer);
	}

	~TetrisWindow() {
	}

private:
	int restartLocalGame(int nbrOfPlayers) {
		tetrisGame_.closeGame();
		std::vector<DevicePtr> devices;

		const int size = devices_.size();
		for (int i = 0; i < nbrOfPlayers && i < size; ++i) {
            devices.push_back(devices_[i]);
		}

		tetrisGame_.createLocalGame(devices);
		tetrisGame_.setReadyGame(true);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
		return devices.size();
	}

	void restartGame() {
		tetrisGame_.setReadyGame(true);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	}

	void createCustomGame(int width, int height, int maxLevel) {

	}

	bool isPaused() const {
		return tetrisGame_.isPaused();
	}

	void setPause(bool pause) {
		if (pause != tetrisGame_.isPaused()) {
			tetrisGame_.pause();
		}
	}

	// Override gui::GuiWindow
	void updateGame(Uint32 deltaTime) {
		tetrisGame_.update(deltaTime);

		glPushMatrix();
		int w = getWidth();
		int h = getHeight() - 30;

		int x, y;
		int width, height;

		// Centers the game and holds the correct proportions. The sides is transparent.
		if (tetrisGame_.getWidth() / w > tetrisGame_.getHeight() / h) {
			// Black sides, up and down.
			double scale = w / tetrisGame_.getWidth();
			glTranslated(0, (h - scale*tetrisGame_.getHeight()) * 0.5, 0);
			glScaled(scale, scale, 1);
			x = 0;
			y = static_cast<int>((h - scale*tetrisGame_.getHeight()) * 0.5);
			width = w;
			height = static_cast<int>(scale*tetrisGame_.getHeight());
		} else {
			// Black sides, left and right.
			double scale = h / tetrisGame_.getHeight();
			glTranslated((w-scale*tetrisGame_.getWidth()) * 0.5, 0, 0);
			glScaled(scale,scale,1);
			x = static_cast<int>((w-scale*tetrisGame_.getWidth()) * 0.5);
			y = 0;
			width = static_cast<int>(scale*tetrisGame_.getWidth());
			height = h;
		}

		glEnable(GL_SCISSOR_TEST);
		glScissor(x,y,width,height);
		tetrisGame_.draw();
		glPopMatrix();

		glDisable(GL_SCISSOR_TEST);
	}

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent) override {
		for (DevicePtr& device : devices_) {
			device->eventUpdate(windowEvent);
		}
	}

	void handleConnectionEvent(NetworkEventPtr nEvent) {
		/*
		if (connectionEvent == ManagerEvent::STARTS_GAME) {
		}
		*/
		if (std::shared_ptr<GameOver> gameOver = std::dynamic_pointer_cast<GameOver>(nEvent)) {
            HighscorePtr highscore = getHighscorePtr();
            // Points high enough to be saved in the highscore list?
            if (highscore->isNewRecord(gameOver->points_)) {
                // Set points in order for highscore to know which point to save in list.
                highscore->setNextRecord(gameOver->points_);
                // In order for the user to insert name.
                highscore->click();
            }
		}
	}

	void loadHighscore() {
        std::string line;
        std::ifstream file("highscore");
        if (file.is_open()) {
            HighscorePtr highscore = getHighscorePtr();
            while (file.good()) {
                std::getline(file, line);
                //std::stringstream strStream;
                int record = -1;
                std::string name = "";
                std::string date = "";

                highscore->setNextRecord(record);
                highscore->addNewRecord(name, date);
            }
            file.close();
        }
	}

	TetrisGame tetrisGame_;
	std::vector<DevicePtr> devices_;
};

#endif // TETRISWINDOW_H
