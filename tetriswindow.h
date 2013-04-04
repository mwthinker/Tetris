#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "gamesprite.h"
#include "gamefont.h"

#include "tetrisboard.h"

#include "tetrisgame.h"

#include "menuevent.h"

#include "color.h"

#include "glcoordinate.h"

#include "gui/guiwindow.h"

#include <mw/gamewindow.h>
#include <mw/sprite.h>

//#include <guiwindow.h>
#include <SDL_opengl.h>
#include <iostream>

struct View {
public:
    int x, y;
    int width;
    int height;
};

enum Frame {FRAME_MENU,
			FRAME_LOCAL_GAME,
			FRAME_SERVER_CREATE,
			FRAME_SERVER_LOOBY,
			FRAME_SERVER_GAME,
			FRAME_CLIENT_CREATE,
			FRAME_CLIENT_LOOBY,
			FRAME_CLIENT_GAME};

class TetrisWindow : public gui::GuiWindow {
public:
    TetrisWindow() {
		numberOfPlayer_ = 1;
		tetrisGame_.createLocalGame(1);
		tetrisGame_.setReadyGame(true);
		tetrisGame_.startGame();		

		tetrisGame_.setOnConnectionEvent([&](Protocol::ManagerEvent connectionEvent) {
			handleConnectionEvent(connectionEvent);
		});
    }

	~TetrisWindow() {
	}

private:
	void restartGame() {
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	}

	void createCustomGame(int width, int height, int maxLevel) {

	}

    // Override gui::GuiWindow
    void updateGame(Uint32 deltaTime) {
		tetrisGame_.physicUpdate(deltaTime);
		View gameView;
		glPushMatrix();
        int w = getWidth();
		int h = getHeight() - 30;

        // Centers the game and holds the correct proportions. The sides is transparent.
        if (tetrisGame_.getWidth() / w > tetrisGame_.getHeight() / h) {
            // Black sides, up and down.
            double scale = w / tetrisGame_.getWidth();
            glTranslated(0, (h - scale*tetrisGame_.getHeight()) * 0.5, 0);
            glScaled(scale, scale, 1);
            gameView.x = 0;
            gameView.y = static_cast<int>((h - scale*tetrisGame_.getHeight()) * 0.5);
            gameView.width = w;
            gameView.height = static_cast<int>(scale*tetrisGame_.getHeight());
        } else {
            // Black sides, left and right.
            double scale = h / tetrisGame_.getHeight();
            glTranslated((w-scale*tetrisGame_.getWidth()) * 0.5, 0, 0);
            glScaled(scale,scale,1);
            gameView.x = static_cast<int>((w-scale*tetrisGame_.getWidth()) * 0.5);
            gameView.y = 0;
            gameView.width = static_cast<int>(scale*tetrisGame_.getWidth());
            gameView.height = h;
        }

		glEnable(GL_SCISSOR_TEST);
		glScissor(gameView.x,gameView.y,gameView.width,gameView.height);
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
