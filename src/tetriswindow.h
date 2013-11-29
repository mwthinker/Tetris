#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "guiwindow.h"
#include "tetrisgame.h"
#include "device.h"

class TetrisWindow : public GuiWindow {
public:
	TetrisWindow();
	~TetrisWindow();

private:
    void setNbrOfHumanPlayers(int number) override;
	int getNbrOfHumanPlayers() const override;
	void setNbrOfComputerPlayers(int number) override;
	int getNbrOfComputerPlayers() const override;

	void abortGame() override;
	void startGame() override;
	void restartGame() override;

	void createLocalGame() override;
	void createLocalGame(int width, int height, int maxLevel) override;
	void createServerGame(int port, int width, int height) override;
	void createClientGame(int port, std::string ip) override;

	bool isPaused() const override;
	void changePauseState() override;
	bool isReady() const override;
	void changeReadyState() override;

	// Override gui::GuiWindow
	void updateGame(Uint32 deltaTime) override;

	// Override gui::GuiWindow
	void drawGame(Uint32 deltaTime) override;

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent) override;

	void handleConnectionEvent(NetworkEventPtr nEvent);

	void loadHighscore();
	void saveHighscore() override;

	TetrisGame tetrisGame_;
	std::vector<DevicePtr> devices_;
	int nbrOfHumanPlayers_, nbrOfComputerPlayers_;
};

#endif // TETRISWINDOW_H