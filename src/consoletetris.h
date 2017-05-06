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

#include <rlutil.h>

#include <array>

class ConsoleTetris {
public:
	ConsoleTetris(TetrisEntry tetrisEntry);

	void startLoop();

private:
	enum Mode {MENU = 0, GAME = 1, CUSTOM_GAME = 2, NETWORK_GAME = 3, HIGHSCORE = 4, QUIT = 5, SIZE = 6};

	void printMenu(Mode option);	

	static void draw(int x, int y, std::string text);

	static void draw(int x, int y, std::string text, int color);

	static void drawClear(int x, int y, std::string text, int color);

	void handleConnectionEvent(TetrisGameEvent& tetrisEvent);

	void update(double deltaTime, double time);

	void moveMenuUp();
	void moveMenuDown();

	void execute(Mode option);

	TetrisGame tetrisGame_;
	TetrisEntry tetrisEntry_;
	Mode mode_;
	Mode option_;
	std::shared_ptr<ConsoleKeyboard> keyboard1_;
	std::map<int, ConsoleGraphic> graphicPlayers_;
	std::array<char, 100 * 100> screen_;
};

#endif // CONSOLETETRIS_H
