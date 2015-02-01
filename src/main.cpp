#include "tetriswindow.h"
#include "tetrisentry.h"

#include <sstream>

int main (int argc, char** argv) {	
	TetrisEntry tetrisEntry("tetris.xml");

	int menuIndex = 0;

	if (argc >= 2) {
		std::stringstream stream;
		stream << *(argv + 1);
		stream >> menuIndex;
	}

	if (tetrisEntry.isError()) {
		tetrisEntry.printError();
	} else {
		TetrisWindow game(tetrisEntry.getChildEntry("tetris"), menuIndex);
		game.startLoop();
	}
	
    return 0;
}
