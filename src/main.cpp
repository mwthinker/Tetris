#include "tetriswindow.h"
#include "tetrisentry.h"
#include "consoletetris.h"

#include <sstream>
#include <iostream>

int main (int argc, char** argv) {	
	TetrisEntry tetrisEntry("tetris.xml");

	if (tetrisEntry.isError()) {
		tetrisEntry.printError();
		return 1;
	}

	if (argc >= 2) {
		std::string programName = *argv;
		std::string code(*(argv + 1));		
		if (code == "-h" || code == "--help") {
			std::cout << "Usage: " << programName << " OPTIONS \n\n";
			std::cout << "OPTIONS:\n";
			std::cout << "\t-h --help                show this help\n";
			std::cout << "\t-n --no-window           no window and only in the terminal\n\n";
			std::cout << "Starts a tetris game.\n\n";
			std::cout << "Example: \n";
			std::cout << "\t" << "Starts a tetrig game without a window. \n";
			std::cout << "\t" << programName << " -n" << std::endl;
			return 0;
		} else if (code == "-n" || code == "--no-window") {
			ConsoleTetris game(tetrisEntry.getChildEntry("tetris"));
			game.startLoop();
			return 0;
		}
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
