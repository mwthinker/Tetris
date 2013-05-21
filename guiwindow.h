#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guitypedefs.h"
#include "multiframe.h"
#include "highscore.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <tuple>
#include <string>
#include <algorithm>
#include <functional>

class GuiWindow : public mw::Window {
public:
	GuiWindow();

protected:
	SDLKey pauseKey_;
	SDLKey restartKey_;

    HighscorePtr getHighscorePtr() const;
	gui::TextButtonPtr getPausePtr() const;

	virtual void setNumberOfLocalPlayers(int number) = 0;
	virtual int getNumberOfLocalPlayers() const = 0;

private:
	virtual void saveHighscore() = 0;

	bool isUpdatingGame() const {
		return multiFrame_.getCurrentFrameIndex() == playFrameIndex_;
	}

	bool isDrawGame() const;
	void setDrawGame(bool drawGame);

	virtual void updateGame(Uint32 deltaTime) {
	}

	virtual void updateGameEvent(const SDL_Event& windowEvent) {
	}

	virtual void createCustomGame(int width, int height, int maxLevel) {
	}

    virtual void createLocalGame() = 0;
	virtual void createServerGame(int port) = 0;
	virtual void createClientGame(int port, std::string ip) = 0;

	virtual bool isPaused() const = 0;

	virtual void setPause(bool pause) = 0;

	virtual void restartGame() {
	}

	void quit();

	gui::TextButtonPtr createButton(std::string text, int size, std::function<void(gui::GuiItem*)> onClick);
	gui::TextBoxPtr createTextBox(int size);
	gui::BarColorPtr createUpperBar();

	void initFrameMenu();
	void initPlayFrame();
	void initHighscoreFrame();
	void initCustomPlayFrame();
	void initOptionFrame();
	void initCreateServerFrame();
	void initCreateClientFrame();
	void initServerLoobyFrame();
	void initClientLoobyFrame();

	void initNewHighScoreFrame();

	void resize(int width, int height) override;

	// Override mw::Window
	void update(Uint32 deltaTime) override;

	// Override mw::Window
	void eventUpdate(const SDL_Event& windowEvent) override;

	int hDistance_;

	gui::MultiFrame multiFrame_;
	bool fixSize_;

	int playFrameIndex_;
	int networkFrameIndex_;
	int highscoreFrameIndex_;
	int customFrameIndex_;
	int optionFrameIndex_;
	int newHighscoreFrameIndex_;
	int createClientFrameIndex_;
	int createServerFrameIndex_;
	int loobyClientFrameIndex_;
	int loobyServerFrameIndex_;

	// Attribute defined in initCreateServerMenu.
	gui::TextBoxPtr portBox_, ipBox_;;

    // Attribute defined in initHighscoreMenu.
	HighscorePtr highscorePtr_;

	// Attribute defined in initFrameMenu.
	gui::ButtonPtr resumeButton_;

	// Attribute defined in initPlayFrame.
	gui::TextButtonPtr pause_;

	// Attributes defined in initCustomPlayFrame.
	gui::TextBoxPtr customPlayWidth_, customPlayHeight_, customPlaymaxLevel_;

	// Attributes defined in initNewHighScoreFrame.
	gui::TextBoxPtr nameBox_;
};

#endif // GUIWINDOW_H
