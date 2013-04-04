#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisboard.h"
#include "tetrisgame.h"
#include "gui/color.h"
#include "gui/guiwindow.h"

#include <mw/gamewindow.h>
#include <mw/sprite.h>

#include <iostream>

class TetrisWindow : public gui::GuiWindow {
public:
    TetrisWindow() {
		numberOfPlayer_ = 1;
		tetrisGame_.createLocalGame(1);

		tetrisGame_.setOnConnectionEvent([&](Protocol::ManagerEvent connectionEvent) {
			handleConnectionEvent(connectionEvent);
		});
    }

	~TetrisWindow() {
	}

private:
	void restartGame() {
		tetrisGame_.setReadyGame(true);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	}

	void createCustomGame(int width, int height, int maxLevel) {

	}

	void pauseGame() {
		tetrisGame_.pause();
	}

    // Override gui::GuiWindow
    void updateGame(Uint32 deltaTime) {
		tetrisGame_.physicUpdate(deltaTime);
		
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
        tetrisGame_.graphicUpdate(deltaTime);
        glPopMatrix();

		glDisable(GL_SCISSOR_TEST);
    }

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent) {
		tetrisGame_.eventUpdate(windowEvent);
		switch (windowEvent.type) {
		case SDL_QUIT:
			//quit();
			break;
		case SDL_KEYDOWN:
			switch (windowEvent.key.keysym.sym) {
			case SDLK_t:
				mw::Window::setWindowsSize(100,100);
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

	void handleConnectionEvent(Protocol::ManagerEvent connectionEvent) {
		if (connectionEvent == PlayerManager::ManagerEvent::STARTS_GAME) {
			PlayerManager::Status status = tetrisGame_.getStatus();
			std::cout << "MENUEVENT"<<std::endl;
			switch (status) {
			case PlayerManager::Status::LOCAL:
				//gui_.pushEvent(MENUEVENT_LOCALGAME);
				std::cout << "MENUEVENT_LOCALGAMEDAS"<<std::endl;
				break;
			case PlayerManager::Status::SERVER:
				//gui_.pushEvent(MENUEVENT_SERVERGAME);
				std::cout << "MENUEVENT_SERVERGAME"<<std::endl;
				break;
			case PlayerManager::Status::CLIENT:
				//gui_.pushEvent(MENUEVENT_CLIENTGAME);
				break;
			}
		}
	}

    TetrisGame tetrisGame_;
	mw::Text header_;

	int numberOfPlayer_;
};

#endif // TETRISWINDOW_H
