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
    void setNumberOfLocalPlayers(int number) override;
	int getNumberOfLocalPlayers() const override;

	void abortGame() override;
	void startGame() override;
	void restartGame() override;
	void createCustomGame(int width, int height, int maxLevel) override;
	void createLocalGame() override;
	void createServerGame(int port) override;
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
	int numberOfLocalPlayers_;
};

#endif // TETRISWINDOW_H
