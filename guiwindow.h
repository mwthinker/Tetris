#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guitypedefs.h"
#include "multiframe.h"

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

private:
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

	virtual bool isPaused() const = 0;

	virtual void setPause(bool pause) = 0;

	virtual int restartLocalGame(int nbrOfPlayers) = 0;

	virtual void restartGame() {
	}

	void quit();

	gui::ButtonPtr createButton(std::string text, int size, std::function<void(gui::GuiItem*)> onClick);
	gui::TextBoxPtr createTextBox(int size);
	gui::BarColorPtr createUpperBar();

	void initFrameMenu();
	void initPlayFrame();
	void initHighscoreFrame();
	void initCustomPlayFrame();
	void initOptionFrame();
	void initCreateClientFrame();

	void resize(int width, int height) override;

	// Override mw::Window
	void update(Uint32 deltaTime) override;

	// Override mw::Window
	void eventUpdate(const SDL_Event& windowEvent) override;

	int hDistance_;

	gui::MultiFrame multiFrame_;
	bool fixSize_;

	int playFrameIndex_;
	int highscoreFrameIndex_;
	int customFrameIndex_;
	int optionFrameIndex_;

	// Attribute defined in initFrameMenu.
	gui::ButtonPtr resumeButton_;

	// Attributes defined in initCustomPlayFrame.
	gui::TextBoxPtr customPlayWidth_, customPlayHeight_, customPlaymaxLevel_;
};

#endif // GUIWINDOW_H
