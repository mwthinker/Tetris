#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include "guitypedefs.h"
#include "multiframe.h"
#include "highscore.h"
#include "manbutton.h"
#include "networklooby.h"
#include "device.h"
#include "tetrisparameters.h"
#include "ai.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>
#include <mw/color.h>

#include <string>
#include <array>
#include <functional>
#include <queue>

class GuiWindow : public mw::Window {
protected:
	GuiWindow();

	void initOptionFrame(const std::vector<DevicePtr>& devices);

	SDL_Keycode pauseKey_;
	SDL_Keycode restartKey_;

    HighscorePtr getHighscorePtr() const;
	gui::TextButtonPtr getPausePtr() const;
	NetworkLoobyPtr getNetworkLoobyPtr() const;
	gui::TextButtonPtr getReadyPtr() const;	

	void gotoLocalPlayFrame();
	void gotoServerPlayFrame();
	void gotoClientPlayFrame();

	void gotoServerLoobyFrame();
	void gotoClientLoobyFrame();

	inline Ai getAi1() const {
		return activeAis_[0];
	}

	inline Ai getAi2() const {
		return activeAis_[1];
	}

	inline Ai getAi3() const {
		return activeAis_[2];
	}

	inline Ai getAi4() const {
		return activeAis_[3];
	}

private:
	//--------------------- Is to be derived. -----------------------------
	virtual void saveHighscore() = 0;
	virtual void updateGame(Uint32 deltaTime) = 0;
	virtual void drawGame(Uint32 deltaTime) = 0;
	virtual void updateGameEvent(const SDL_Event& windowEvent) = 0;

	virtual void setNbrOfHumanPlayers(int number) = 0;
	virtual int getNbrOfHumanPlayers() const = 0;
	virtual void setNbrOfComputerPlayers(int number) = 0;
	virtual int getNbrOfComputerPlayers() const = 0;

	// Aborts the current game.
    virtual void abortGame() = 0;
	
	// Creates a local game whith default settings.
	virtual void createLocalGame() = 0;
	// Creates a local game with the settings defined in parameterts.
    virtual void createLocalGame(int width, int height, int maxLevel) = 0;
	// Creates a server with the game settings defined in parameterts.
	virtual void createServerGame(int port, int width, int height) = 0;
	// Creates a client with the game settings defined at the remote server.
	virtual void createClientGame(int port, std::string ip) = 0;

	// Returns true if the game is paused else false.
	virtual bool isPaused() const = 0;	
	// Change the current pause state.
	virtual void changePauseState() = 0;	
	// Returns true if the user is ready to for the game to start.
	virtual bool isReady() const = 0;
	// Change the current ready state.
	virtual void changeReadyState() = 0;

	// Restarts the current game.
	virtual void restartGame() = 0;
	// Starts a new game. Will only effect if the game is able to start.
	virtual void startGame() = 0;

	//---------------------------------------------------------------------
	void loadAllSettings();
	void saveAllSettings();
	bool isUpdatingGame() const;
	bool isDrawGame() const;
	void setDrawGame(bool drawGame);
	
	gui::TextButtonPtr createButton(std::string text, int width,
		std::function<void(gui::GuiItem*)> onClick);
	gui::TextBoxPtr createTextBox(int width);
	gui::BarColorPtr createUpperBar();

	void initManButtons();

	void initFrameMenu();
	void initPlayFrame();
	void initHighscoreFrame();
	void initCustomPlayFrame();
	void initCreateServerFrame();
	void initCreateClientFrame();
	void initServerLoobyFrame();
	void initClientLoobyFrame();
	void initWaitToConnectFrame();
	void initNetworkPlayFrame();
	void initAiFrame();
	void initNewHighScoreFrame();

	void resize(int width, int height);

	// Override mw::Window
	void update(Uint32 deltaTime) override;

	// Override mw::Window
	void eventUpdate(const SDL_Event& windowEvent) override;
	
	int hDistance_;

	gui::MultiFrame multiFrame_;
	std::queue<SDL_Event> eventQueue_;

	int playFrameIndex_, networkFrameIndex_, highscoreFrameIndex_,
		customFrameIndex_, optionFrameIndex_, newHighscoreFrameIndex_,
		createClientFrameIndex_, createServerFrameIndex_, loobyClientFrameIndex_,
		loobyServerFrameIndex_, waitToConnectFrameIndex_, networkPlayFrameIndex_,
		aiFrameIndex_;

	// Attributes defined in initManButton.
	ManButtonPtr humanButton_, computerButton_;

	// Attributes defined in initCreateServerMenu.
	gui::TextBoxPtr portBox_, ipBox_;

    // Attribute defined in initHighscoreMenu.
	HighscorePtr highscorePtr_;

	// Attribute defined in initFrameMenu.
	gui::ButtonPtr resumeButton_;

	// Attribute defined in initPlayFrame.
	gui::TextButtonPtr pause_;

	// Init in initServerLoobyFrame.
	NetworkLoobyPtr networkLoobyPtr_;
	gui::TextButtonPtr ready_;

	// Attributes defined in initCustomPlayFrame.
	gui::TextBoxPtr customPlayWidth_, customPlayHeight_, customPlaymaxLevel_;

	// Attributes defined in initNewHighScoreFrame.
	gui::TextBoxPtr nameBox_;

	// Attributes defined in initAiFrame.
	gui::TextButtonPtr ai1Button_, ai2Button_, ai3Button_, ai4Button_;

	// All ai:s.
	std::array<Ai, 4> activeAis_;
	std::vector<Ai> ais_;

	mw::Color barColor_, textButtonColor_, textColor_, focusColor_,
		onHoverColor_, notHoverColor_, pushedColor_;
};

#endif // GUIWINDOW_H
