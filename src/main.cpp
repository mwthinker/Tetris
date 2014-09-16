#include "tetriswindow.h"
#include "tetrisentry.h"

int main (int argc, char** argv) {	
	TetrisEntry tetrisEntry("tetris.xml");

	TetrisWindow game(tetrisEntry.getChildEntry("tetris"));
	mw::Window::startLoop();
	
    return 0;
}
