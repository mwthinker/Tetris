#include "consoletetris.h"

#include "tetrisparameters.h"
#include "tetrisgameevent.h"
#include "device.h"
#include "tetrisgame.h"
#include "tetrisboard.h"
#include "computer.h"
#include "consolegraphic.h"
#include "consolekeyboard.h"

#include "tetrisdata.h"

#include <algorithm>

using namespace console;

ConsoleTetris::ConsoleTetris() :
	keyboard1_(std::make_shared<ConsoleKeyboard>("Keyboard 1", console::Key::DOWN, console::Key::LEFT, console::Key::RIGHT, console::Key::UP, console::Key::KEY_DELETE)),
	keyboard2_(std::make_shared<ConsoleKeyboard>("Keyboard 2", console::Key::KEY_S, console::Key::KEY_A, console::Key::KEY_D, console::Key::KEY_W, console::Key::KEY_Q)),
	mode_(MENU), option_(GAME),
	humanPlayers_(1), aiPlayers_(0) {

	tetrisGame_.addCallback(std::bind(&ConsoleTetris::handleConnectionEvent, this, std::placeholders::_1));
}

void ConsoleTetris::initPreLoop() {
	// Init ai players.
	activeAis_[0] = findAiDevice(TetrisData::getInstance().getAi1Name());
	activeAis_[1] = findAiDevice(TetrisData::getInstance().getAi2Name());
	activeAis_[2] = findAiDevice(TetrisData::getInstance().getAi3Name());

	Console::setCursorVisibility(false);
	printMainMenu();
}

DevicePtr ConsoleTetris::findAiDevice(std::string name) const {
	auto ais = TetrisData::getInstance().getAiVector();
	ais.push_back(Ai()); // Add default ai.

	for (const Ai& ai : ais) {
		if (ai.getName() == name) {
			return std::make_shared<Computer>(ai);
		}
	}
	return std::make_shared<Computer>(ais.back());
}

void ConsoleTetris::printGameMenu() {
	Console::setCursorPosition(0, 0);
	Console::setTextColor(console::Color::RED);
	Console::setBackgroundColor(console::Color::BLACK);
	if (tetrisGame_.isPaused()) {
		print("Menu [Key 1]    Restart [Key 2]    Human -/+ [Key 3/4]    AI -/+ [Key 5/6]    Unpause [P]");
	} else {
		print("Menu [Key 1]    Restart [Key 2]    Human -/+ [Key 3/4]    AI -/+ [Key 5/6]    Pause [P]   ");
	}
}

void ConsoleTetris::printGame() {
	printGameMenu();
	for (auto& player : graphicPlayers_) {
		player.second.drawStatic();
	}
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
					mode_ = QUIT;
					quit();
					break;
			}
			break;
	}

	switch (mode_) {
		case GAME:
			switch (consoleEvent.type) {
				case console::ConsoleEventType::KEYDOWN:
					switch (consoleEvent.keyEvent.key) {
						case console::Key::KEY_1:
							mode_ = MENU;
							printMainMenu();
							break;
						case console::Key::KEY_2:
							restartCurrentGame();
							break;
						case console::Key::KEY_3:
							humanPlayers_ = (humanPlayers_ + 2) % 3; // Remove one human player.
							restartCurrentGame();
							break;
						case console::Key::KEY_4:
							humanPlayers_ = (humanPlayers_ + 1) % 3; // Add a human player by one.
							restartCurrentGame();
							break;
						case console::Key::KEY_5:
							aiPlayers_ = (aiPlayers_ + activeAis_.size()) % (activeAis_.size() + 1); // Remove one ai player.
							restartCurrentGame();
							break;
						case console::Key::KEY_6:
							aiPlayers_ = (aiPlayers_ + 1) % (activeAis_.size() + 1); // Add a ai player by one.
							restartCurrentGame();
							break;
						case console::Key::KEY_P:
							tetrisGame_.pause();
							break;
					}
					break;
				case console::ConsoleEventType::CONSOLERESIZE:
					switch (mode_) {
						case MENU:
							printMainMenu();
							break;
						case GAME:
							printGame();
							break;
						case QUIT:
							break;
						default:
							break;
					}
					break;
			}

			keyboard1_->eventUpdate(consoleEvent);
			keyboard2_->eventUpdate(consoleEvent);
			break;
		case MENU:
			switch (consoleEvent.type) {
				case console::ConsoleEventType::KEYDOWN:
					switch (consoleEvent.keyEvent.key) {
						case console::Key::KEY_1:
							option_ = GAME;
							execute(option_);
							break;
						case console::Key::KEY_2:
							option_ = CUSTOM_GAME;
							execute(option_);
							break;
						case console::Key::KEY_3:
							option_ = NETWORK_GAME;
							execute(option_);
							break;
						case console::Key::KEY_4:
							option_ = HIGHSCORE;
							execute(option_);
							break;
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

void ConsoleTetris::restartCurrentGame() {
	// Initialization local game settings.	
	std::vector<DevicePtr> devices;
	if (humanPlayers_ == 1) {
		devices.push_back(keyboard1_);
	} else if (humanPlayers_ == 2) {
		devices.push_back(keyboard1_);
		devices.push_back(keyboard2_);
	}
	for (int i = 0; i < aiPlayers_; ++i) {
		devices.push_back(activeAis_[i]);
	}

	tetrisGame_.setPlayers(TETRIS_WIDTH, TETRIS_HEIGHT, devices);
	tetrisGame_.createLocalGame();
}

void ConsoleTetris::printMainMenu() {
	clear();
	Console::setTextColor(console::Color::RED);
	Console::setBackgroundColor(console::Color::BLACK);
	print("MWetris, Use arrow to move, ESC to quit.");
	draw(2, 2, "MWetris", console::Color::WHITE);
	draw(5, 7, "Play [1]", GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 8, "Custom play [2]", CUSTOM_GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 9, "Network game [3]", NETWORK_GAME == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 10, "Highscore [4]", HIGHSCORE == option_ ? console::Color::RED : console::Color::WHITE);
	draw(5, 11, "Exit [ESQ]", QUIT == option_ ? console::Color::RED : console::Color::WHITE);
}

void ConsoleTetris::draw(int x, int y, std::string text) {
	setCursorPosition(x, y);
	print(text);
}

void ConsoleTetris::draw(int x, int y, std::string text, console::Color color) {
	setTextColor(color);
	setBackgroundColor(console::Color::BLACK);
	setCursorPosition(x, y);
	print(text);
}

void ConsoleTetris::drawClear(int x, int y, std::string text, console::Color color) {
	std::string remove;
	for (char key : text) {
		remove += " ";
	}
	setTextColor(color);
	setCursorPosition(x, y);
	print(remove);
	setCursorPosition(x, y);
	print(text);
}

void ConsoleTetris::handleConnectionEvent(TetrisGameEvent& tetrisEvent) {
	try {
		auto& levelChange = dynamic_cast<LevelChange&>(tetrisEvent);
		graphicPlayers_[levelChange.player_->getId()].updateLevel(levelChange.newLevel_);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& pointsChange = dynamic_cast<PointsChange&>(tetrisEvent);
		graphicPlayers_[pointsChange.player_->getId()].updateLevel(pointsChange.newPoints_);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);
		printGame();
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& newConnection = dynamic_cast<NewConnection&>(tetrisEvent);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& initGameVar = dynamic_cast<InitGame&>(tetrisEvent);
		graphicPlayers_.clear();

		int delta = 2;
		for (auto& player : initGameVar.players_) {
			auto& graphic = graphicPlayers_[player->getId()];
			graphic.restart(*player, delta, 2, this);
			delta += graphic.getWidth();
		}
		clear();
		printGame();
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
	if (nbr >= ENUM_SIZE) {
		option_ = GAME;
	} else {
		option_ = (TetrisMenu) nbr;
	}
	printMainMenu();
}

void ConsoleTetris::moveMenuUp () {
	int nbr = (int) option_;
	--nbr;
	if (nbr <= 0) {
		option_ = (TetrisMenu) (ENUM_SIZE - 1);
	} else {
		option_ = (TetrisMenu) nbr;
	}
	printMainMenu();
}

void ConsoleTetris::execute(TetrisMenu option) {
	clear();
	switch (option) {
	case GAME:
		mode_ = GAME;
		restartCurrentGame();
		break;
	case QUIT:
		mode_ = QUIT;
		quit();
		break;
	default:
		break;
	}
}
