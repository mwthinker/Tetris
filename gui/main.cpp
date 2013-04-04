#include "guiwindow.h"

class TetrisWindow : public gui::GuiWindow {
private:
	void updateGame(Uint32 deltaTime) override {
	}

	void updateGameEvent(const SDL_Event& windowEvent) override {
	}

	void createCustomGame(int width, int height, int maxLevel) override {
	}

	void pauseGame() override {
	}
	
	void restartGame() override {
	}
};

int main (int argc, char** argv) {
    TetrisWindow gui;
	gui.startLoop();
	
    return 0;
}