#include "tetriswindow.h"
#include "tetrisentry.h"

#include <sstream>

int main (int argc, char** argv) {	
	TetrisEntry tetrisEntry("tetris.xml");

	if (tetrisEntry.isError()) {
		tetrisEntry.printError();
		return 1;
	}

	int menuIndex = 0;
	if (argc >= 2) {
		std::stringstream stream;
		stream << *(argv + 1);
		if (!(stream >> menuIndex)) {
			if (strcmp(*(argv + 1), "server") == 0) {
				int port = 11155;
				if (argc >= 3) {
					std::stringstream stream(*(argv + 2));
					stream >> port;
				}
				TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
				game.startServer(port);
				game.startLoop();
				return 0;
			} else if (strcmp(*(argv + 1), "client") == 0) {
				if (argc >= 3) {
					int port = 11155;
					if (argc >= 4) {
						std::stringstream stream(*(argv + 3));
						stream >> port;
					}
					TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
					game.startClient(port, *(argv + 2));
					game.startLoop();
					return 0;
				}
			}
		}
	}
	
	TetrisWindow game(tetrisEntry.getChildEntry("tetris"), menuIndex);
	game.startLoop();
	
    return 0;
}
