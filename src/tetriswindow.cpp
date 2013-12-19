#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"
#include "computer.h"
#include "textbutton.h"

#include <gui/borderlayout.h>

//#include "textbutton.h"
//#include "highscore.h"

#include <mw/sprite.h>
#include <mw/color.h>

#include <iostream>
#include <fstream>
#include <sstream>

namespace {
	
	gui::Panel* createBackgroundPanel() {
		gui::Panel* panel = new gui::Panel();
		panel->setBackground(spriteBackground.getTexture());
		panel->setLayout(new gui::BorderLayout);
		return panel;
	}

}

TetrisWindow::TetrisWindow() {
    nbrOfHumanPlayers_ = 1;
	nbrOfComputerPlayers_ = 0;
	tetrisGame_.addCallback([&](NetworkEventPtr nEvent) {
		handleConnectionEvent(nEvent);
	});

	// Initializes default keybord devices for two players.
	DevicePtr device1(new Keyboard("Keyboard 1", SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP));
	devices_.push_back(device1);
	DevicePtr device2(new Keyboard("Keyboard 2", SDLK_s, SDLK_a, SDLK_d, SDLK_w));
	devices_.push_back(device2);

	setDefaultClosing(true);

	// Init joysticks!
	/*
	const std::vector<mw::JoystickPtr>& joystics = mw::Joystick::getJoystics();
	for (const mw::JoystickPtr& joystick : joystics) {
		std::cout << joystick->getName() << std::endl;
		DevicePtr device(new Joystick(joystick, 0, 1));
		devices_.push_back(device);
	}
	*/

	// Create all frames.
	getCurrentPanel()->setBackground(spriteBackground.getTexture());	
	optionFrameIndex_ = getCurrentPanelIndex();
	
	playFrameIndex_ = push_back(createBackgroundPanel());
	highscoreFrameIndex_ = push_back(createBackgroundPanel());
	customFrameIndex_ = push_back(createBackgroundPanel());
	newHighscoreFrameIndex_ = push_back(createBackgroundPanel());
	networkFrameIndex_ = push_back(createBackgroundPanel());
	createClientFrameIndex_ = push_back(createBackgroundPanel());
	createServerFrameIndex_ = push_back(createBackgroundPanel());
	loobyClientFrameIndex_ = push_back(createBackgroundPanel());
	loobyServerFrameIndex_ = push_back(createBackgroundPanel());
	waitToConnectFrameIndex_ = push_back(createBackgroundPanel());
	networkPlayFrameIndex_ = push_back(createBackgroundPanel());
	aiFrameIndex_ = push_back(createBackgroundPanel());
	
	//loadHighscore();
	initOptionFrame(devices_);
}

gui::Panel* TetrisWindow::createPlayOptions() const {
	gui::Panel* panel = new gui::Panel;
	panel->setBackgroundColor(mw::Color(1, 1, 1, 0));
	
	TextButton* b1 = new TextButton("Play", fontDefault35);
	panel->add(b1);
	TextButton* b2 = new TextButton("Custom play", fontDefault35);
	panel->add(b2);
	TextButton* b3 = new TextButton("Highscore", fontDefault35);
	panel->add(b3);
	TextButton* b4 = new TextButton("Options", fontDefault35);
	panel->add(b4);
	TextButton* b5 = new TextButton("Exit", fontDefault35);
	panel->add(b5);

	panel->setPreferredSize(400, 400);

	return panel;
}

TetrisWindow::~TetrisWindow() {
}

void TetrisWindow::initOptionFrame(const std::vector<DevicePtr>& devices) {
	setCurrentFrame(optionFrameIndex_);
	gui::Panel* optionsPanel = createPlayOptions();
	add(optionsPanel, gui::BorderLayout::NORTH);
}

void TetrisWindow::setNbrOfHumanPlayers(int number) {
    const int size = devices_.size();
	if (number < 0) {
		nbrOfHumanPlayers_ = 0;
	} else if (number <= size) {
        nbrOfHumanPlayers_ = number;
    } else {
        nbrOfHumanPlayers_ = size;
    }
}

int TetrisWindow::getNbrOfHumanPlayers() const {
    return nbrOfHumanPlayers_;
}

void TetrisWindow::setNbrOfComputerPlayers(int number) {
    if (number <= 4) {
        nbrOfComputerPlayers_ = number;
    } else {
        nbrOfComputerPlayers_ = 4;
    }
}

int TetrisWindow::getNbrOfComputerPlayers() const {
    return nbrOfComputerPlayers_;
}

void TetrisWindow::createLocalGame(int width, int height, int maxLevel) {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.setAis(activeAis_[0], activeAis_[1], activeAis_[2], activeAis_[3]);
	tetrisGame_.createLocalGame(tmpDevices, nbrOfComputerPlayers_, width, height, maxLevel);
	tetrisGame_.startGame();
}

void TetrisWindow::createLocalGame() {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.setAis(activeAis_[0], activeAis_[1], activeAis_[2], activeAis_[3]);
	tetrisGame_.createLocalGame(tmpDevices, nbrOfComputerPlayers_);
	tetrisGame_.startGame();
}

void TetrisWindow::createServerGame(int port, int width, int height) {
    const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.setAis(activeAis_[0], activeAis_[1], activeAis_[2], activeAis_[3]);
	tetrisGame_.createServerGame(tmpDevices, nbrOfComputerPlayers_, port, width, height, TETRIS_MAX_LEVEL);
}

void TetrisWindow::createClientGame(int port, std::string ip) {
    const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.setAis(activeAis_[0], activeAis_[1], activeAis_[2], activeAis_[3]);
	tetrisGame_.createClientGame(tmpDevices, nbrOfComputerPlayers_, port, ip, TETRIS_MAX_LEVEL);
}

void TetrisWindow::drawGame(Uint32 deltaTime) {
	glPushMatrix();
	int w = getWidth();
	int h = getHeight() - 30; // Bar size, but should not be magic number!

	// Centers the game and holds the correct proportions. The sides is transparent.
	if (tetrisGame_.getWidth() / w > tetrisGame_.getHeight() / h) {
		// Black sides, up and down.
		double scale = w / tetrisGame_.getWidth();
		glTranslated(0, (h - scale*tetrisGame_.getHeight()) * 0.5, 0);
		glScaled(scale, scale, 1);
	} else {
		// Black sides, left and right.
		double scale = h / tetrisGame_.getHeight();
		glTranslated((w-scale*tetrisGame_.getWidth()) * 0.5, 0, 0);
		glScaled(scale,scale,1);
	}

	tetrisGame_.draw();
	glPopMatrix();
}

void TetrisWindow::updateGameEvent(const SDL_Event& windowEvent) {
	for (DevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

void TetrisWindow::handleConnectionEvent(NetworkEventPtr nEvent) {
	/*
	if (std::shared_ptr<GameOver> gameOver = std::dynamic_pointer_cast<GameOver>(nEvent)) {
		HighscorePtr highscore = getHighscorePtr();
		// Points high enough to be saved in the highscore list?
		if (highscore->isNewRecord(gameOver->points_)) {
			// Set points in order for highscore to know which point to save in list.
			highscore->setNextRecord(gameOver->points_);
			// In order for the user to insert name.
			highscore->click();
		}
	} else if (std::shared_ptr<GamePause> gameOver = std::dynamic_pointer_cast<GamePause>(nEvent)) {
		if (gameOver->pause_) {
			getPausePtr()->setText("Unpause");
		} else {
			getPausePtr()->setText("Pause");
		}
	} else if (std::shared_ptr<NewConnection> newConnection = std::dynamic_pointer_cast<NewConnection>(nEvent)) {
		auto networkLooby = getNetworkLoobyPtr();
		networkLooby->clear();
		newConnection->iterate([&](int id, int nbrOfPlayers, bool ready) {
			networkLooby->addConnection(id,nbrOfPlayers, ready);
		});

		switch (newConnection->status_) {
		case NewConnection::CLIENT:
			gotoClientLoobyFrame();
			break;
		case NewConnection::SERVER:
			gotoServerLoobyFrame();
			break;
		case NewConnection::LOCAL:
			// Is no local looby.
			break;
		}
	} else if (std::shared_ptr<GameStart> start = std::dynamic_pointer_cast<GameStart>(nEvent)) {
		switch (start->status_) {
		case GameStart::LOCAL:
			gotoLocalPlayFrame();
			break;
		case GameStart::CLIENT:
			gotoClientPlayFrame();
			break;
		case GameStart::SERVER:
			gotoServerPlayFrame();
			break;
		}
	} else if (std::shared_ptr<GameReady> ready = std::dynamic_pointer_cast<GameReady>(nEvent)) {
		auto networkLooby = getNetworkLoobyPtr();
		networkLooby->setReady(ready->id_,ready->ready_);
	}
	*/
}

void TetrisWindow::loadHighscore() {
	/*
	std::ifstream file("highscore");
	if (file.is_open()) {
		HighscorePtr highscore = getHighscorePtr();
		while (file.good()) {
			std::string line;
			std::getline(file, line);
			if (line.size() < 6) {
				break;
			}

			// Line should look like.
			// line = "'Marcus' 2013-05-19 2340".
			unsigned pos1 = line.find('\'');
			unsigned pos2 = line.find('\'', pos1+1);
			std::string name = line.substr(pos1+1, pos2-pos1-1); // name = "Marcus".

			line = line.substr(pos2+1); // line = " 2013-05-19 2340".
			std::stringstream strStream(line);
			std::string date;
			int record;
			strStream >> date >> record; // date = "2013-05-19". record = 2340.

			highscore->setNextRecord(record);
			highscore->addNewRecord(name, date);
		}
		file.close();
	}
	*/
}

void TetrisWindow::saveHighscore() {
	/*
	std::string line;
	std::ofstream file("highscore");
	HighscorePtr highscore = getHighscorePtr();
	if (file.is_open() && file.good()) {
		// Line should look like.
		// line = "'Marcus' 2013-05-19 2340".
		highscore->iterateRecords([&](int points,std::string name, std::string date) {
			file << "'" << name << "' " << date << " " << points << "\n";
		});
	}
	file.close();
	*/
}

void TetrisWindow::loadAllSettings() {
	/*
	// Load all ais.
	{
		std::ifstream file("ais/ais");
		if (file.is_open()) {
			ais_.clear();
			ais_.push_back(Ai());
			std::string filename;
			while (std::getline(file, filename)) {
				Ai ai;
				if (loadAi(ai, "ais/", filename)) {
					ais_.push_back(ai);
				}
			}
		}
	}
	// Load all ais that is set to be active.
	{
	std::ifstream file("settings");
	if (file.is_open()) {
		for (Ai& ai : activeAis_) {
			std::string filename;
			std::getline(file, filename);
			loadAi(ai, "ais/", filename);
		}
	}
	*/
}

void TetrisWindow::saveAllSettings() {
	std::ofstream file("settings");
	if (file.is_open() && file.good()) {
		for (const Ai& ai : activeAis_) {
			file << ai.getName() << "\n";
		}
	}
}
