#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "device.h"
#include "textbutton.h"
#include "tetrisgame.h"

#include <gui/frame.h>
#include <gui/textfield.h>

class ManButton;
class Highscore;

class TetrisWindow : public gui::Frame {
public:
	TetrisWindow();
	~TetrisWindow();

private:
	void updateDevices(Frame* frame, const SDL_Event& windowEvent);

	gui::Panel* createMenu();

	void initMenuPanel(const std::vector<DevicePtr>& devices);
	void initPlayPanel();
	void initHighscorePanel();
	void initNewHighscorePanel();
	void initCustomPlayPanel();
	void initCreateServerPanel();
	void initCreateClientPanel();
	void initServerLoobyPanel();
	void initClientLoobyPanel();
	void initWaitToConnectPanel();
	void initNetworkPlayPanel();
	void initAiPanel();
	void initNewHighScorePanel();

	void createLocalGame();
	void createLocalGame(int width, int height, int maxLevel);
	void createServerGame(int port, int width, int height);
	void createClientGame(int port, std::string ip);

	void handleConnectionEvent(NetworkEventPtr nEvent);

	void loadHighscore();
	void saveHighscore();

	void loadAllSettings();
	void saveAllSettings();

	TetrisGame tetrisGame_;

	std::vector<DevicePtr> devices_;
	int nbrOfHumanPlayers_, nbrOfComputerPlayers_;

	ManButton* nbrHumans_;
	ManButton* nbrAis_;
	TextButton* pauseButton_;
	Highscore* highscore_;
	gui::TextField* textField_;
	gui::TextField* widthField_;
	gui::TextField* heightField_;
	gui::TextField* minLevel_;
	gui::TextField* maxLevel_;
	TextButton* networkButton_;
	gui::TextField* ip_;
	gui::TextField* port_;
	std::array<gui::Panel*, 4> players_;

	// All ai:s.
	std::array<Ai, 4> activeAis_;
	std::vector<Ai> ais_;

	// All panels.
	int menuIndex_,
		playIndex_,
		networkIndex_,
		highscoreIndex_,
		customIndex_,
		optionIndex_,
		newHighscoreIndex_,
		createClientIndex_,
		createServerIndex_,
		loobyClientIndex_,
		loobyServerIndex_,
		waitToConnectIndex_,
		networkPlayIndex_,
		aiIndex_;
};

#endif // TETRISWINDOW_H
