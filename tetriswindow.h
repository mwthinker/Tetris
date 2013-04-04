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

		tetrisGame.setOnConnectionEvent([&](Protocol::ManagerEvent connectionEvent) {
			handleConnectionEvent(connectionEvent);
		});
    }

	~TetrisWindow() {
	}

private:
	void createCustomGame(int width, int height, int maxLevel) {

	}

    // Override gui::GuiWindow
    void updateGame(Uint32 deltaTime) {
		glEnable(GL_SCISSOR_TEST);
		glScissor(gameView_.x,gameView_.y,gameView_.width,gameView_.height);
		drawTetrisGame(deltaTime);
		glDisable(GL_SCISSOR_TEST);
    }

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent) {
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

    void drawTetrisGame(Uint32 deltaTime) {
        glPushMatrix();
        int w = getWidth();
		int h = getHeight() - 30;

        // Centers the game and holds the correct proportions. The sides is transparent.
        if (tetrisGame.getWidth() / w > tetrisGame.getHeight() / h) {
            // Black sides, up and down.
            double scale = w / tetrisGame.getWidth();
            glTranslated(0, (h - scale*tetrisGame.getHeight()) * 0.5, 0);
            glScaled(scale, scale, 1);
            gameView_.x = 0;
            gameView_.y = static_cast<int>((h - scale*tetrisGame.getHeight()) * 0.5);
            gameView_.width = w;
            gameView_.height = static_cast<int>(scale*tetrisGame.getHeight());
        } else {
            // Black sides, left and right.
            double scale = h / tetrisGame.getHeight();
            glTranslated((w-scale*tetrisGame.getWidth()) * 0.5, 0, 0);
            glScaled(scale,scale,1);
            gameView_.x = static_cast<int>((w-scale*tetrisGame.getWidth()) * 0.5);
            gameView_.y = 0;
            gameView_.width = static_cast<int>(scale*tetrisGame.getWidth());
            gameView_.height = h;
        }
        tetrisGame.graphicUpdate(deltaTime);
        glPopMatrix();
    }

	void handleConnectionEvent(Protocol::ManagerEvent connectionEvent) {
		if (connectionEvent == PlayerManager::ManagerEvent::STARTS_GAME) {
			PlayerManager::Status status = tetrisGame.getStatus();
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

    TetrisGame tetrisGame;
	mw::Text header_;

	int numberOfPlayer_;

    View gameView_;
};

#endif // TETRISWINDOW_H
