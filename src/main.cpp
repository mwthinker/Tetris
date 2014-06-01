#include "tetriswindow.h"
#include "gamedata.h"

int main (int argc, char** argv) {	
	GameData gameData("tetris.xml");

	TetrisWindow game(gameData);
	mw::Window::startLoop();
	
    return 0;
}
