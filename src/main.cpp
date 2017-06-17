#include "tetriswindow.h"
#include "tetrisentry.h"

#if CONSOLE_TETRIS
#include "consoletetris.h"
#endif // D_CONSOLE_TETRIS

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
#if CONSOLE_TETRIS
			std::cout << "Usage: " << programName << "\n";
#endif // D_CONSOLE_TETRIS
			std::cout << "\t" << programName << " -n" << "\n";
			std::cout << "\t" << programName << " -m [ <MENU_INDEX> ] " << "\n";
			std::cout << "\t" << programName << " -s [ <HOST> [ <PORT> ] ] " << "\n";
			std::cout << "\t" << programName << " -c [ <PORT> ] " << "\n";
			std::cout << "\n";
			std::cout << "Options:\n";
			std::cout << "\t-h --help                show this help\n";
#if CONSOLE_TETRIS
			std::cout << "\t-n --no-window           no window and only in the terminal\n";
#endif // D_CONSOLE_TETRIS
			std::cout << "\t-m --menu-index          start the game in the chosen menu\n";
			std::cout << "\t-s --server              create a server game immediately\n";
			std::cout << "\t-c --client              connect to a host game immediately\n";

			std::cout << "Example: \n";
#if CONSOLE_TETRIS
			std::cout << "\t" << "Starts a tetris game without a window. \n\n";
			std::cout << "\t" << programName << " -n\n\n";
#endif // D_CONSOLE_TETRIS
			std::cout << "Starts a client game.\n\n";
			std::cout << "\t" << programName << " -c 192.168.1.1 11155" << std::endl;
			return 0;
		}
#if CONSOLE_TETRIS
		else if (code == "-n" || code == "--no-window") {
			ConsoleTetris game(tetrisEntry.getChildEntry("tetris"));
			game.startLoop();
			return 0;
		}
#endif // D_CONSOLE_TETRIS
		else if (code == "-s" || code == "--server") {
			int port = 11155;
			if (argc >= 3) {
				std::stringstream stream(*(argv + 2));
				stream >> port;
			}
			TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
			game.startServer(port);
			game.startLoop();
			return 0;
		} else if (code == "-s" || code == "--server") {
			int port = 11155;
			if (argc >= 3) {
				std::stringstream stream(*(argv + 2));
				stream >> port;
			}
			std::string ip = "localhost";
			if (argc >= 4) {
				ip = *(argv + 2);
			}
			TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
			game.startClient(port, ip);
			game.startLoop();
			return 0;
		} else if (code == "-m" || code == "--menu-index") {
			int menuIndex = 0;
			if (argc >= 2) {
				std::stringstream stream;
				stream << *(argv + 1);
				stream >> menuIndex;
			}
			TetrisWindow game(tetrisEntry.getChildEntry("tetris"), menuIndex);
			game.startLoop();
		} else {
			std::cout << "Incorrect argument " << code << "\n";
		}
	} else {
		TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
		game.startLoop();
	}

    return 0;
}
