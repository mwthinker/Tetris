#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"
#include "computer.h"
#include "textbutton.h"

//#include "highscore.h"

#include <gui/borderlayout.h>
#include <gui/flowlayout.h>
#include <gui/verticallayout.h>
#include <gui/gridlayout.h>
#include <gui/Label.h>

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

	gui::Panel* createBar() {
		gui::Panel* bar = new gui::Panel;
		bar->setPreferredSize(35, 35);
		bar->setBackgroundColor(mw::Color(.5, 0, 0, .30));
		bar->setLayout(new gui::FlowLayout(gui::FlowLayout::LEFT, 5, 0));
		return bar;
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
	optionIndex_ = getCurrentPanelIndex();	
	playIndex_ = push_back(createBackgroundPanel());
	highscoreIndex_ = push_back(createBackgroundPanel());
	customIndex_ = push_back(createBackgroundPanel());
	newHighscoreIndex_ = push_back(createBackgroundPanel());
	networkIndex_ = push_back(createBackgroundPanel());
	createClientIndex_ = push_back(createBackgroundPanel());
	createServerIndex_ = push_back(createBackgroundPanel());
	loobyClientIndex_ = push_back(createBackgroundPanel());
	loobyServerIndex_ = push_back(createBackgroundPanel());
	waitToConnectIndex_ = push_back(createBackgroundPanel());
	networkPlayIndex_ = push_back(createBackgroundPanel());
	aiIndex_ = push_back(createBackgroundPanel());
	
	//loadHighscore();
	initOptionFrame(devices_);
	initPlayFrame();
}

gui::Panel* TetrisWindow::createPlayOptions() {
	gui::Panel* panel = new gui::Panel;
	panel->setLayout(new gui::VerticalLayout(5, 15, 10));
	panel->setBackgroundColor(mw::Color(1, 1, 1, 0));

	gui::Label* label = new gui::Label("MWetris", fontDefault50);
	label->setTextColor(mw::Color(1, 1, 1));
	label->setBackgroundColor(mw::Color(1, 1, 1,0));
	panel->add(label);
	panel->setPreferredSize(400, 400);
	
	TextButton* b1 = new TextButton("Play", fontDefault30);
	panel->add(b1);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(playIndex_);
	});
	TextButton* b2 = new TextButton("Custom play", fontDefault30);
	panel->add(b2);
	b2->addActionListener([&](gui::Component*) {
		setCurrentPanel(optionIndex_);
	});
	TextButton* b3 = new TextButton("Highscore", fontDefault30);
	panel->add(b3);
	b3->addActionListener([&](gui::Component*) {
		setCurrentPanel(highscoreIndex_);
	});
	TextButton* b4 = new TextButton("Options", fontDefault30);
	b4->addActionListener([&](gui::Component*) {
		setCurrentPanel(optionIndex_);
	});
	panel->add(b4);
	TextButton* b5 = new TextButton("Exit", fontDefault30);
	b5->addActionListener([&](gui::Component*) {
		Window::quit();
	});
	panel->add(b5);

	return panel;
}

TetrisWindow::~TetrisWindow() {
}

void TetrisWindow::initOptionFrame(const std::vector<DevicePtr>& devices) {
	setCurrentPanel(optionIndex_);
	gui::Panel* optionsPanel = createPlayOptions();
	
	TextButton* b1 = new TextButton("Resume", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(playIndex_);
	});

	gui::Panel* bar = createBar();
	bar->add(b1);

	add(bar, gui::BorderLayout::NORTH);
	add(optionsPanel, gui::BorderLayout::WEST);
}

void TetrisWindow::initPlayFrame() {
	setCurrentPanel(playIndex_);
	
	gui::Panel* bar = createBar();
	bar->setLayout(new gui::GridLayout(1, 2));

	gui::Panel* p1 = new gui::Panel;
	gui::Panel* p2 = new gui::Panel;
	// Make panels transparent.
	p1->setBackgroundColor(mw::Color(1, 1, 1, 0));
	p2->setBackgroundColor(p1->getBackgroundColor());
	p1->setLayout(new gui::FlowLayout(gui::FlowLayout::LEFT, 5, 0));
	p2->setLayout(new gui::FlowLayout(gui::FlowLayout::RIGHT, 5, 0));

	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(optionIndex_);
	});
	p1->add(b1);

	TextButton* b2 = new TextButton("Restart", fontDefault30);
	p1->add(b2);

	TextButton* b3 = new TextButton("Pause", fontDefault30);
	p2->add(b3);
	
	p1->setPreferredSize(b1->getPreferredSize() + b2->getPreferredSize());
	p2->setPreferredSize(b2->getPreferredSize());

	bar->add(p1);
	bar->add(p2);
	add(bar, gui::BorderLayout::NORTH);
	//setCurrentPanel(optionIndex_);
}

/*
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
*/

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
