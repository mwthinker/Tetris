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
	keyboard2_(std::make_shared<ConsoleKeyboard>("Keyboard 2", 's', 'a', 'd', 'w', 'f')) {

	tetrisGame_.addCallback(std::bind(&ConsoleTetris::handleConnectionEvent, this, std::placeholders::_1));
}

void ConsoleTetris::startLoop() {
	rlutil::hidecursor();
	rlutil::saveDefaultColor();
	rlutil::setColor(2);
	printf("MWetris Use arrow to move, ESC to quit.\n");
	rlutil::setColor(6);
	//rlutil::anykey("Hit any key to start.\n");

	auto time = std::chrono::high_resolution_clock::now();
	bool quit = false;

	// Initialization local game settings.
	std::vector<DevicePtr> devices_ = {std::make_shared<Computer>(), std::make_shared<Computer>()};
	tetrisGame_.setPlayers(devices_);
	tetrisGame_.createLocalGame();

	while (!quit) {
		auto currentTime = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> delta = currentTime - time;

		rlutil::cls();
		update(delta.count());
		time = currentTime;

		if (kbhit()) {
			char key = rlutil::getkey();

			keyboard1_->eventUpdate(key);
			keyboard2_->eventUpdate(key);

			if (key == rlutil::KEY_ESCAPE) {
				quit = true;
			}
			std::cout << key;
		}
		rlutil::msleep(100);
	}

	rlutil::cls();
	rlutil::resetColor();
	rlutil::showcursor();
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

		for (auto& player : initGameVar.players_) {
			auto& graphic = graphicPlayers_[player->getId()];
			graphic.restart(*player, tetrisEntry_.getDeepChildEntry("window"));
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

void ConsoleTetris::update(double deltaTime) {
	// Drawing
	rlutil::cls();

	int w = 5;
	for (auto& pair : graphicPlayers_) {
		ConsoleGraphic& graphic = pair.second;
		graphic.draw(deltaTime, w, 2);
		w += graphic.getWidth();
	}

	tetrisGame_.update((int) (deltaTime * 1000));
}
