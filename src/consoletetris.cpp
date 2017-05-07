#include "consoletetris.h"

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

ConsoleTetris::ConsoleTetris(TetrisEntry tetrisEntry) :
	tetrisEntry_(tetrisEntry),
	keyboard1_(std::make_shared<ConsoleKeyboard>("Keyboard 1", rlutil::KEY_DOWN, rlutil::KEY_LEFT, rlutil::KEY_RIGHT, rlutil::KEY_UP, '-')),
	mode_(MENU), option_(GAME) {

	tetrisGame_.addCallback(std::bind(&ConsoleTetris::handleConnectionEvent, this, std::placeholders::_1));
}

void ConsoleTetris::startLoop() {
	rlutil::hidecursor();
	rlutil::saveDefaultColor();
	rlutil::setColor(2);
	printf("MWetris, Use arrow to move, ESC to quit.\n");

	auto time = std::chrono::high_resolution_clock::now();
	double gameTime = 0;

	printMenu(option_);
	while (mode_ != QUIT) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = currentTime - time;
		gameTime += delta.count();

		//rlutil::cls();
		update(delta.count(), gameTime);
		time = currentTime;

		rlutil::msleep(10);
	}

	rlutil::cls();
	rlutil::resetColor();
	rlutil::showcursor();
}

void ConsoleTetris::printMenu(Mode option) {
	draw(2, 2, "MWetris", rlutil::WHITE);
	draw(5, 7, "Play [1]", GAME == option_ ? rlutil::RED : rlutil::WHITE);
	draw(5, 8, "Custom play [2]", CUSTOM_GAME == option_ ? rlutil::RED : rlutil::WHITE);
	draw(5, 9, "Network game [3]", NETWORK_GAME == option_ ? rlutil::RED : rlutil::WHITE);
	draw(5, 10, "Highscore [4]", HIGHSCORE == option_ ? rlutil::RED : rlutil::WHITE);
	draw(5, 11, "Exit [ESQ]", QUIT == option_ ? rlutil::RED : rlutil::WHITE);
}

void ConsoleTetris::draw(int x, int y, std::string text) {
	rlutil::locate(x, y);
	std::cout << text;
}

void ConsoleTetris::draw(int x, int y, std::string text, int color) {
	rlutil::setColor(color);
	rlutil::locate(x, y);
	std::cout << text;
}

void ConsoleTetris::drawClear(int x, int y, std::string text, int color) {
	std::string remove;
	for (char key : text) {
		remove += " ";
	}
	rlutil::setColor(color);
	rlutil::locate(x, y);
	std::cout << remove;
	rlutil::locate(x, y);
	std::cout << text;
}

void ConsoleTetris::handleConnectionEvent(TetrisGameEvent& tetrisEvent) {
	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);

		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);

		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& newConnection = dynamic_cast<NewConnection&>(tetrisEvent);

		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& initGameVar = dynamic_cast<InitGame&>(tetrisEvent);
		graphicPlayers_.clear();
		rlutil::cls();

		int delta = 2;
		for (auto& player : initGameVar.players_) {
			auto& graphic = graphicPlayers_[player->getId()];
			graphic.restart(*player, tetrisEntry_.getDeepChildEntry("window"), delta, 2);
			graphic.drawStatic();
			delta += graphic.getWidth();
		}

		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& start = dynamic_cast<GameStart&>(tetrisEvent);
		switch (start.status_) {
			case GameStart::LOCAL:
				break;
			case GameStart::CLIENT:
				break;
			case GameStart::SERVER:
				break;
		}
		return;
	} catch (std::bad_cast exp) {}
}

void ConsoleTetris::update(double deltaTime, double time) {
	switch (mode_) {
		case GAME:
			{				
				for (auto& pair : graphicPlayers_) {
					ConsoleGraphic& graphic = pair.second;
					//graphic.draw(deltaTime);
				}
			}
			tetrisGame_.update(deltaTime);

			if (kbhit()) {
				char key = rlutil::getkey();
				keyboard1_->eventUpdate(key, time);

				if (key == rlutil::KEY_ESCAPE) {
					mode_ = QUIT;
				}
			}
			break;
		case MENU:
			if (kbhit()) {
				char key = rlutil::getkey();

				switch (key) {
					case rlutil::KEY_DOWN:
						moveMenuDown();
						break;
					case rlutil::KEY_UP:
						moveMenuUp();
						break;
					case rlutil::KEY_ENTER:
						execute(option_);
						break;
					case rlutil::KEY_ESCAPE:
						mode_ = QUIT;
						break;
					default:
						break;
				}
			}
			break;
	}
}

void ConsoleTetris::moveMenuDown() {
	int nbr = (int) option_;
	++nbr;
	if (nbr >= Mode::SIZE) {
		option_ = (Mode) 1;
	} else {
		option_ = (Mode) nbr;
	}
	printMenu(option_);
}

void ConsoleTetris::moveMenuUp () {
	int nbr = (int)option_;
	--nbr;
	if (nbr <= 0) {
		option_ = (Mode) (Mode::SIZE - 1);
	} else {
		option_ = (Mode) nbr;
	}
	printMenu(option_);
}

void ConsoleTetris::execute(Mode option) {
	rlutil::cls();
	switch (option) {
	case GAME:
	{
		mode_ = Mode::GAME;
		// Initialization local game settings.	
		std::vector<DevicePtr> devices = { keyboard1_, std::make_shared<Computer>() };
		tetrisGame_.setPlayers(devices);
		tetrisGame_.createLocalGame();
	}
		break;
	case QUIT:
		mode_ = QUIT;
		break;
	default:
		break;
	}
}
