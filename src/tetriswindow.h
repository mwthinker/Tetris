#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "tetrisgame.h"
#include "device.h"

#include <gui/frame.h>

class TetrisWindow : public gui::Frame {
public:
	TetrisWindow();
	~TetrisWindow();

private:
	void initOptionFrame(const std::vector<DevicePtr>& devices);

    void setNbrOfHumanPlayers(int number);
	int getNbrOfHumanPlayers() const;
	void setNbrOfComputerPlayers(int number);
	int getNbrOfComputerPlayers() const;	

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
};

#endif // TETRISWINDOW_H
