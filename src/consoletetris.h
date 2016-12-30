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

class ConsoleTetris {
public:
	ConsoleTetris(TetrisEntry tetrisEntry);

	void startLoop();

private:
	void handleConnectionEvent(TetrisGameEvent& tetrisEvent);

	void update(double deltaTime);

	TetrisGame tetrisGame_;
	TetrisEntry tetrisEntry_;
	std::shared_ptr<ConsoleKeyboard> keyboard1_, keyboard2_;
	std::map<int, ConsoleGraphic> graphicPlayers_;	
};

#endif // CONSOLETETRIS_H
