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
	int restartLocalGame(int nbrOfPlayers) override;
	void restartGame() override;
	void createCustomGame(int width, int height, int maxLevel) override;
	
	bool isPaused() const override;
	void setPause(bool pause) override;

	// Override gui::GuiWindow
	void updateGame(Uint32 deltaTime) override;

	// Override gui::GuiWindow
	void updateGameEvent(const SDL_Event& windowEvent) override;

	void handleConnectionEvent(NetworkEventPtr nEvent);
	
	void loadHighscore();
	void saveHighscore() override;

	TetrisGame tetrisGame_;
	std::vector<DevicePtr> devices_;
};

#endif // TETRISWINDOW_H
