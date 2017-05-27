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
			std::cout << "Usage: " << programName << " OPTIONS [11155=SERVER_PORT] [localhost=IP_TO_SERVER]\n\n";
			std::cout << "OPTIONS:\n";
			std::cout << "\t-h --help                show this help\n";
#if CONSOLE_TETRIS
			std::cout << "\t-n --no-window           no window and only in the terminal\n\n";
#endif // D_CONSOLE_TETRIS
			std::cout << "\t-m --menu-index          start the game in the chosen menu, where the next argument is the menu index\n\n";
			std::cout << "\t-s --server              start a server game immediately\n\n";
			std::cout << "\t-c --client              start a client game immediately, next argument it the port,\n";
			std::cout << "\t                         and the next after that is the the ip to the server\n\n";

			std::cout << "Starts a tetris game.\n\n";
			std::cout << "Example: \n";
			std::cout << "\t" << "Starts a tetris game without a window. \n";
			std::cout << "\t" << programName << " -n\n\n";
			std::cout << "Starts a client game.\n\n";
			std::cout << "Example: \n";
			std::cout << "\t" << programName << " -c 192.168.1.1 port" << std::endl;

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
			TetrisWindow game(tetrisEntry.getChildEntry("tetris"), 0);
			game.startLoop();
		}
	}

    return 0;
}
