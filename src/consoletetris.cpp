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

using namespace console;

ConsoleTetris::ConsoleTetris(TetrisEntry tetrisEntry) :
	tetrisEntry_(tetrisEntry),
	keyboard1_(std::make_shared<ConsoleKeyboard>("Keyboard 1", console::Key::DOWN, console::Key::LEFT, console::Key::RIGHT, console::Key::UP, console::Key::PAGEDOWN)),
	mode_(MENU), option_(GAME) {

	tetrisGame_.addCallback(std::bind(&ConsoleTetris::handleConnectionEvent, this, std::placeholders::_1));
}

void ConsoleTetris::initPreLoop() {
	Console::setTextColor(console::Color::RED);
	Console::setCursorVisibility(false);
	printf("MWetris, Use arrow to move, ESC to quit.\n");
	printMenu(option_);
}

void ConsoleTetris::update(double deltaTime) {
	switch (mode_) {
		case GAME:
			tetrisGame_.update(deltaTime);
			break;
	}
	sleep(1.0 / 60.0);
}

void ConsoleTetris::eventUpdate(console::ConsoleEvent& consoleEvent) {
	switch (consoleEvent.type) {
		case console::ConsoleEventType::KEYDOWN:
			switch (consoleEvent.keyEvent.key) {
				case console::Key::ESCAPE: // Fall through.
				case console::Key::SPACE:
					quit();
					break;
			}
			break;
	}

	switch (mode_) {
		case GAME:			
			keyboard1_->eventUpdate(consoleEvent);
			break;
		case MENU:
			switch (consoleEvent.type) {
				case console::ConsoleEventType::KEYDOWN:
					switch (consoleEvent.keyEvent.key) {
						case console::Key::DOWN:
							moveMenuDown();
							break;
						case console::Key::UP:
							moveMenuUp();
							break;
						case console::Key::RETURN:
							execute(option_);
							break;
					}
					break;
			}
			break;
	}
}

void ConsoleTetris::printMenu(Mode option) {
	draw(2, 2, "MWetris", console::Color::WHITE);
	draw(5, 7, "Play [1]", GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 8, "Custom play [2]", CUSTOM_GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 9, "Network game [3]", NETWORK_GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 10, "Highscore [4]", HIGHSCORE == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 11, "Exit [ESQ]", QUIT == option_ ? console::Color::RED : console::Color::WHITE);
}

void ConsoleTetris::draw(int x, int y, std::string text) {
	setCursorPosition(x, y);
	*this << text;
}

void ConsoleTetris::draw(int x, int y, std::string text, console::Color color) {
	setTextColor(color);
	setCursorPosition(x, y);
	*this << text;
}

void ConsoleTetris::drawClear(int x, int y, std::string text, console::Color color) {
	std::string remove;
	for (char key : text) {
		remove += " ";
	}
	setTextColor(color);
	setCursorPosition(x, y);
	*this << remove;
	setCursorPosition(x, y);
	*this << text;
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
		clear();

		int delta = 2;
		for (auto& player : initGameVar.players_) {
			auto& graphic = graphicPlayers_[player->getId()];
			graphic.restart(*player, tetrisEntry_.getDeepChildEntry("window"), delta, 2, this);
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
	clear();
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
