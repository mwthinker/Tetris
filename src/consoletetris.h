#ifndef CONSOLETETRIS_H
#define CONSOLETETRIS_H

#include "tetrisparameters.h"
#include "tetrisentry.h"
#include "tetrisgameevent.h"
#include "device.h"
#include "tetrisgame.h"
#include "tetrisboard.h"
#include "computer.h"
#include "consolegraphic.h"
#include "consolekeyboard.h"
#include "device.h"
#include "ai.h"

#include <console/console.h>

#include <array>

class ConsoleTetris : public console::Console {
public:
	ConsoleTetris(TetrisEntry tetrisEntry);

protected:
	void initPreLoop() override;
private:
	enum Mode {MENU = 0, GAME = 1, CUSTOM_GAME = 2, NETWORK_GAME = 3, HIGHSCORE = 4, QUIT = 5, ENUM_SIZE = 6};

	void update(double deltaTime) override;
	
	void eventUpdate(console::ConsoleEvent& consoleEvent) override;

	void printMainMenu();

	void printGameMenu();

	void printGame();

	void draw(int x, int y, std::string text);

	void draw(int x, int y, std::string text, console::Color color);

	void drawClear(int x, int y, std::string text, console::Color color);

	void handleConnectionEvent(TetrisGameEvent& tetrisEvent);

	void moveMenuUp();
	void moveMenuDown();

	void restartCurrentGame();

	void execute(Mode option);

	TetrisGame tetrisGame_;
	TetrisEntry tetrisEntry_;

	// All ai:s.
	std::array<Ai, 4> activeAis_;
	std::vector<Ai> ais_;

	Mode mode_;
	Mode option_;
	std::shared_ptr<ConsoleKeyboard> keyboard1_, keyboard2_;
	int humanPlayers_;
	std::vector<DevicePtr> activePlayers_;
	std::map<int, ConsoleGraphic> graphicPlayers_;
	std::array<char, 100 * 100> screen_;
};

#endif // CONSOLETETRIS_H
