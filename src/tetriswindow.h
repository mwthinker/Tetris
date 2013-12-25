#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "device.h"
#include "textbutton.h"
#include "tetrisgame.h"

#include <gui/frame.h>

class TetrisWindow : public gui::Frame {
public:
	TetrisWindow();
	~TetrisWindow();

private:
	gui::Panel* createPlayOptions();

	void initOptionFrame(const std::vector<DevicePtr>& devices);
	void initPlayFrame();

	void createLocalGame();
	void createLocalGame(int width, int height, int maxLevel);
	void createServerGame(int port, int width, int height);
	void createClientGame(int port, std::string ip);

	// Override gui::GuiWindow
	void drawGame(Uint32 deltaTime);

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent);

	void handleConnectionEvent(NetworkEventPtr nEvent);

	void loadHighscore();
	void saveHighscore();

	void loadAllSettings();
	void saveAllSettings();

	TetrisGame tetrisGame_;

	std::vector<DevicePtr> devices_;
	int nbrOfHumanPlayers_, nbrOfComputerPlayers_;

	// All ai:s.
	std::array<Ai, 4> activeAis_;
	std::vector<Ai> ais_;

	// All panels.
	int optionIndex_,
		playIndex_,
		networkIndex_,
		highscoreIndex_,
		customIndex_,
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
