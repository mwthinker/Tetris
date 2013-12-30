#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"
#include "computer.h"
#include "textbutton.h"
#include "manbutton.h"
#include "highscore.h"
#include "gamecomponent.h"
#include "joystick.h"
#include "networklooby.h"

#include <gui/borderlayout.h>
#include <gui/flowlayout.h>
#include <gui/verticallayout.h>
#include <gui/gridlayout.h>
#include <gui/label.h>
#include <gui/textfield.h>

#include <mw/sprite.h>
#include <mw/color.h>
#include <mw/joystick.h>

#include <ctime>
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

	gui::Panel* createPanel(float preferredWidth = 100, float preferredHeight = 100) {
		gui::Panel* panel = new gui::Panel;
		panel->setBackgroundColor(mw::Color(1, 1, 1, 0));
		panel->setPreferredSize(preferredWidth, preferredHeight);
		return panel;
	}

	gui::Label* createLabel(std::string text, mw::FontPtr font) {
		gui::Label* label = new gui::Label(text, font);
		label->setTextColor(mw::Color(1, 1, 1));
		label->setBackgroundColor(mw::Color(1, 1, 1, 0));
		return label;
	}

	void createPlayersFields(std::array<gui::Panel*, 4>&  players) {
		for (unsigned int i = 0; i < players.size(); ++i) {
			players[i] = createPanel(400, 35);
			std::stringstream stream;
			stream << "Human " << i + 1;
			players[i]->add(createLabel(stream.str(), fontDefault18));
			players[i]->add(new gui::TextField(stream.str(), fontDefault18));
			players[i]->setVisible(false);
		}
	}

	void createHumanFields(ManButton* humans, std::array<gui::Panel*, 4>& humanPanels) {
		for (unsigned int i = 0; i < humanPanels.size(); ++i) {
			if (i < humans->getNbr()) {
				humanPanels[i]->setVisible(true);
			} else {
				humanPanels[i]->setVisible(false);
			}
		}
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
	const std::vector<mw::JoystickPtr>& joystics = mw::Joystick::getJoystics();
	for (const mw::JoystickPtr& joystick : joystics) {
		std::cout << joystick->getName() << std::endl;
		DevicePtr device(new Joystick(joystick, 0, 1));
		devices_.push_back(device);
	}
	addSdlEventListener(std::bind(&TetrisWindow::updateDevices, this, this, std::placeholders::_2));

	// Create all frames.
	getCurrentPanel()->setBackground(spriteBackground.getTexture());
	menuIndex_ = getCurrentPanelIndex();
	playIndex_ = push_back(createBackgroundPanel());
	highscoreIndex_ = push_back(createBackgroundPanel());
	customIndex_ = push_back(createBackgroundPanel());
	optionIndex_ = push_back(createBackgroundPanel());
	newHighscoreIndex_ = push_back(createBackgroundPanel());
	networkIndex_ = push_back(createBackgroundPanel());
	createClientIndex_ = push_back(createBackgroundPanel());
	createServerIndex_ = push_back(createBackgroundPanel());
	loobyClientIndex_ = push_back(createBackgroundPanel());
	loobyServerIndex_ = push_back(createBackgroundPanel());
	waitToConnectIndex_ = push_back(createBackgroundPanel());
	networkPlayIndex_ = push_back(createBackgroundPanel());
	aiIndex_ = push_back(createBackgroundPanel());

	initMenuPanel(devices_);
	initPlayPanel();
	initHighscorePanel();
	initNewHighscorePanel();
	initCustomPlayPanel();
	initCreateServerPanel();
	initCreateClientPanel();
	initServerLoobyPanel();
	initClientLoobyPanel();
	initWaitToConnectPanel();
	initNetworkPlayPanel();
	initAiPanel();

	// Init local game settings.
	tetrisGame_.closeGame();
	tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + 1), 0, TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);
	tetrisGame_.startGame();

	setCurrentPanel(menuIndex_);

	loadAllSettings();
	loadHighscore();
}

void TetrisWindow::updateDevices(Frame* frame, const SDL_Event& windowEvent) {
	for (DevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

gui::Panel* TetrisWindow::createMenu() {
	gui::Panel* panel = createPanel(400, 400);
	panel->setLayout(new gui::VerticalLayout(5, 15, 10));
	panel->setBackgroundColor(mw::Color(1, 1, 1, 0));

	gui::Label* label = createLabel("MWetris", fontDefault50);
	panel->add(label);

	TextButton* b1 = new TextButton("Play", fontDefault30);
	panel->add(b1);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(playIndex_);
	});

	TextButton* b2 = new TextButton("Custom play", fontDefault30);
	panel->add(b2);
	b2->addActionListener([&](gui::Component*) {
		setCurrentPanel(customIndex_);
	});

	TextButton* b3 = new TextButton("Network play", fontDefault30);
	panel->add(b3);
	b3->addActionListener([&](gui::Component*) {
		setCurrentPanel(createServerIndex_);
	});

	TextButton* b4 = new TextButton("Highscore", fontDefault30);
	panel->add(b4);
	b4->addActionListener([&](gui::Component*) {
		setCurrentPanel(highscoreIndex_);
	});

	TextButton* b5 = new TextButton("Options", fontDefault30);
	b5->addActionListener([&](gui::Component*) {
		setCurrentPanel(optionIndex_);
	});

	panel->add(b5);
	TextButton* b6 = new TextButton("Exit", fontDefault30);
	b6->addActionListener([&](gui::Component*) {
		Window::quit();
	});
	panel->add(b6);

	return panel;
}

TetrisWindow::~TetrisWindow() {
}

void TetrisWindow::initMenuPanel(const std::vector<DevicePtr>& devices) {
	setCurrentPanel(menuIndex_);

	TextButton* b1 = new TextButton("Resume", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(playIndex_);
	});

	gui::Panel* bar = createBar();
	bar->add(b1);

	add(bar, gui::BorderLayout::NORTH);
	add(createMenu(), gui::BorderLayout::WEST);
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

	gui::Panel* bar = createBar();
	bar->setLayout(new gui::GridLayout(1, 2));

	gui::Panel* p1 = createPanel();
	gui::Panel* p2 = createPanel();
	// Make panels transparent.
	p1->setLayout(new gui::FlowLayout(gui::FlowLayout::LEFT, 5, 0));
	p2->setLayout(new gui::FlowLayout(gui::FlowLayout::RIGHT, 5, 0));

	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});
	p1->add(b1);

	TextButton* b2 = new TextButton("Restart", fontDefault30);
	b2->addActionListener([&](gui::Component*) {
		tetrisGame_.restartGame();
	});
	p1->add(b2);
	pauseButton_ = new TextButton("Pause", fontDefault30);
	pauseButton_->addActionListener([&](gui::Component*) {
		tetrisGame_.pause();
	});
	p2->add(pauseButton_);

	nbrHumans_ = new ManButton(devices_.size(), spriteMan, spriteCross);
	nbrHumans_->addActionListener([&](gui::Component*) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 20, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});
	p1->add(nbrHumans_);
	nbrAis_ = new ManButton(4, spriteComputer, spriteCross);
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component*) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 20, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});
	p2->add(nbrAis_);

	p1->setPreferredSize(b1->getPreferredSize() + b2->getPreferredSize());
	p2->setPreferredSize(b2->getPreferredSize());

	bar->add(p1);
	bar->add(p2);
	add(bar, gui::BorderLayout::NORTH);
	add(new GameComponent(tetrisGame_), gui::BorderLayout::CENTER);
	addKeyListener([&](gui::Component* c, const SDL_Event& keyEvent) {
		switch (keyEvent.type) {
			case SDL_KEYDOWN:
				switch (keyEvent.key.keysym.sym) {
					case SDLK_F12:
						tetrisGame_.restartGame();
						break;
					case SDLK_p:
						pauseButton_->doAction();
						break;
				}
				break;
		}
	});
}

void TetrisWindow::initHighscorePanel() {
	setCurrentPanel(highscoreIndex_);

	gui::Panel* bar = createBar();
	//bar->setLayout(new gui::GridLayout(1, 2));

	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	add(bar, gui::BorderLayout::NORTH);
	highscore_ = new Highscore(10, mw::Color(1, 1, 1));
	add(highscore_, gui::BorderLayout::CENTER);
}

void TetrisWindow::initNewHighscorePanel() {
	setCurrentPanel(newHighscoreIndex_);
	add(createBar(), gui::BorderLayout::NORTH);

	gui::Panel* panel = createPanel(200, 200);
	add(panel, gui::BorderLayout::CENTER);

	panel->add(createLabel("Name: ", fontDefault18));

	textField_ = new gui::TextField(fontDefault18);
	textField_->addActionListener([&](gui::Component* c) {
		gui::TextField* textField = static_cast<gui::TextField*>(c);
		std::string name = textField->getText();

		bool validName = false;
		for (char chr : name) {
			if (chr != ' ') {
				validName = true;
				break;
			}
		}

		if (validName) {
			std::time_t t = std::time(NULL);
			char mbstr[30];
			std::strftime(mbstr, 30, "%Y-%m-%d", std::localtime(&t));
			highscore_->addNewRecord(name, mbstr);
			setCurrentPanel(optionIndex_);
		}
	});
	panel->add(textField_);
	TextButton* button = new TextButton("Ok", fontDefault18);
	button->addActionListener([&](gui::Component* c) {
		textField_->doAction();
	});
	panel->add(button);
}

void TetrisWindow::initCustomPlayPanel() {
	setCurrentPanel(customIndex_);
	gui::Panel* bar = createBar();
	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});

	bar->add(b1);
	add(bar, gui::BorderLayout::NORTH);

	gui::Panel* p1 = createPanel(450, 100);
	p1->add(createLabel("Width", fontDefault18));
	customWidthField_ = new gui::TextField("10", fontDefault18);
	p1->add(customWidthField_);
	p1->add(createLabel("Height", fontDefault18));
	customHeightField_ = new gui::TextField("20", fontDefault18);
	p1->add(customHeightField_);

	gui::Panel* p2 = createPanel(100, 100);
	p2->add(createLabel("Min Level", fontDefault18));
	customMinLevel_ = new gui::TextField("1", fontDefault18);
	p2->add(customMinLevel_);
	p2->add(createLabel("Max Level", fontDefault18));
	customMaxLevel_ = new gui::TextField("20", fontDefault18);
	p2->add(customMaxLevel_);

	gui::Panel* centerPanel = createPanel();
	centerPanel->setLayout(new gui::VerticalLayout);

	centerPanel->add(p1);
	centerPanel->add(p2);

	TextButton* button = new TextButton("Play", fontDefault30);
	centerPanel->add(button);

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initCreateServerPanel() {
	setCurrentPanel(createServerIndex_);
	gui::Panel* bar = createBar();
	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	TextButton* b2 = new TextButton("Client", fontDefault30);
	b2->addActionListener([&](gui::Component*) {
		setCurrentPanel(createClientIndex_);
	});
	bar->add(b2);

	add(bar, gui::BorderLayout::NORTH);

	gui::Panel* centerPanel = createPanel();
	centerPanel->setLayout(new gui::VerticalLayout);

	gui::Panel* p1 = createPanel(450, 40);
	p1->add(createLabel("Width", fontDefault18));
	serverWidthField_ = new gui::TextField("10", fontDefault18);
	p1->add(serverWidthField_);
	p1->add(createLabel("Height", fontDefault18));
	serverHeightField_ = new gui::TextField("20", fontDefault18);
	p1->add(serverHeightField_);
	centerPanel->add(p1);

	gui::Panel* p2 = createPanel(100, 150);
	p2->add(createLabel("Min Level", fontDefault18));
	serverMinLevel_ = new gui::TextField("1", fontDefault18);
	p2->add(serverMinLevel_);
	p2->add(createLabel("Max Level", fontDefault18));
	serverMaxLevel_ = new gui::TextField("20", fontDefault18);
	p2->add(serverMaxLevel_);
	centerPanel->add(p2);

	gui::Panel* p3 = createPanel(450, 40);
	p3->add(createLabel("Port", fontDefault18));
	portServer_ = new gui::TextField("11155", fontDefault18);
	p3->add(portServer_);
	centerPanel->add(p3);

	gui::Panel* p4 = createPanel(450, 40);
	p4->add(createLabel("Local players", fontDefault18));
	centerPanel->add(p4);

	nbrHumansServer_ = new ManButton(devices_.size(), spriteMan, spriteCross);
	p4->add(nbrHumansServer_);
	nbrAisServer_ = new ManButton(activeAis_.size(), spriteComputer, spriteCross);
	p4->add(nbrAisServer_);

	createPlayersFields(playersServer_);
	createHumanFields(nbrHumansServer_, playersServer_);

	for (gui::Panel* panel : playersServer_) {
		centerPanel->add(panel);
	}

	nbrHumansServer_->addActionListener([&](gui::Component* c) {
		for (unsigned int i = 0; i < playersServer_.size(); ++i) {
			if (i < nbrHumansServer_->getNbr()) {
				playersServer_[i]->setVisible(true);
			} else {
				playersServer_[i]->setVisible(false);
			}
		}
	});

	TextButton* button = new TextButton("Connect", fontDefault30);
	centerPanel->add(button);

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initCreateClientPanel() {
	setCurrentPanel(createClientIndex_);
	gui::Panel* bar = createBar();
	TextButton* b1 = new TextButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	TextButton* b2 = new TextButton("Client", fontDefault30);
	b2->addActionListener([&](gui::Component*) {
		setCurrentPanel(createServerIndex_);
	});
	bar->add(b2);

	add(bar, gui::BorderLayout::NORTH);

	gui::Panel* centerPanel = createPanel();
	centerPanel->setLayout(new gui::VerticalLayout);

	gui::Panel* p1 = createPanel(450, 40);
	p1->add(createLabel("Ip", fontDefault18));
	ipClient_ = new gui::TextField("", fontDefault18);
	p1->add(ipClient_);
	p1->add(createLabel("Port", fontDefault18));
	portClient_ = new gui::TextField("11155", fontDefault18);
	p1->add(portClient_);
	centerPanel->add(p1);

	gui::Panel* p2 = createPanel(450, 40);
	p2->add(createLabel("Local players", fontDefault18));
	centerPanel->add(p2);

	nbrHumansClient_ = new ManButton(devices_.size(), spriteMan, spriteCross);
	p2->add(nbrHumansClient_);
	nbrAisClient_ = new ManButton(activeAis_.size(), spriteComputer, spriteCross);
	p2->add(nbrAisClient_);

	createPlayersFields(playersClient_);

	createHumanFields(nbrHumansClient_, playersClient_);
	for (gui::Panel* panel : playersClient_) {
		centerPanel->add(panel);
	}

	nbrHumansClient_->addActionListener([&](gui::Component* c) {
		for (unsigned int i = 0; i < playersClient_.size(); ++i) {
			if (i < nbrHumansClient_->getNbr()) {
				playersClient_[i]->setVisible(true);
			} else {
				playersClient_[i]->setVisible(false);
			}
		}
	});

	TextButton* button = new TextButton("Connect", fontDefault30);
	centerPanel->add(button);

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initServerLoobyPanel() {
	setCurrentPanel(loobyServerIndex_);
	gui::Panel* bar = createBar();
	TextButton* b1 = new TextButton("Abort", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});

	serverLooby_ = new NetworkLooby;

	add(bar, gui::BorderLayout::NORTH);
	add(serverLooby_, gui::BorderLayout::CENTER);
}

void TetrisWindow::initClientLoobyPanel() {
	setCurrentPanel(loobyClientIndex_);
	gui::Panel* bar = createBar();
	TextButton* b1 = new TextButton("Abort", fontDefault30);
	b1->addActionListener([&](gui::Component*) {
		setCurrentPanel(menuIndex_);
	});

	clientLooby_ = new NetworkLooby;

	add(bar, gui::BorderLayout::NORTH);
	add(clientLooby_, gui::BorderLayout::CENTER);
}

void TetrisWindow::initWaitToConnectPanel() {
	//setCurrentPanel(waitToConnectIndex_);
}

void TetrisWindow::initNetworkPlayPanel() {
	//setCurrentPanel(networkPlayIndex_);
}

void TetrisWindow::initAiPanel() {
	//setCurrentPanel(aiIndex_);
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

void TetrisWindow::handleConnectionEvent(NetworkEventPtr nEvent) {
	if (std::shared_ptr<GameOver> gameOver = std::dynamic_pointer_cast<GameOver>(nEvent)) {
		// Points high enough to be saved in the highscore list?
		if (highscore_->isNewRecord(gameOver->points_)) {
			// Set points in order for highscore to know which point to save in list.
			highscore_->setNextRecord(gameOver->points_);
			// In order for the user to insert name.
			setCurrentPanel(newHighscoreIndex_);
		}
	} else if (std::shared_ptr<GamePause> gameOver = std::dynamic_pointer_cast<GamePause>(nEvent)) {
		if (gameOver->pause_) {
			pauseButton_->setLabel("Unpause");
			pauseButton_->setPreferedSizeFitText();
		} else {
			pauseButton_->setLabel("Pause");
			pauseButton_->setPreferedSizeFitText();
		}
	} else if (std::shared_ptr<NewConnection> newConnection = std::dynamic_pointer_cast<NewConnection>(nEvent)) {
		switch (newConnection->status_) {
			case NewConnection::CLIENT:
				clientLooby_->clear();
				newConnection->iterate([&](int id, int nbrOfPlayers, bool ready) {
					clientLooby_->addConnection(id, nbrOfPlayers, ready);
				});
				setCurrentPanel(loobyClientIndex_);
				break;
			case NewConnection::SERVER:
				serverLooby_->clear();
				newConnection->iterate([&](int id, int nbrOfPlayers, bool ready) {
					serverLooby_->addConnection(id, nbrOfPlayers, ready);
				});
				setCurrentPanel(loobyServerIndex_);
				break;
			case NewConnection::LOCAL:
				// Is no local looby.
				break;
		}
	} else if (std::shared_ptr<GameStart> start = std::dynamic_pointer_cast<GameStart>(nEvent)) {
		switch (start->status_) {
			case GameStart::LOCAL:
				break;
			case GameStart::CLIENT:
				break;
			case GameStart::SERVER:
				break;
		}
		setCurrentPanel(playIndex_);
	} else if (std::shared_ptr<GameReady> ready = std::dynamic_pointer_cast<GameReady>(nEvent)) {
		serverLooby_->setReady(ready->id_, ready->ready_);
		clientLooby_->setReady(ready->id_, ready->ready_);
	}
}

void TetrisWindow::loadHighscore() {
	std::ifstream file("highscore");
	if (file.is_open()) {
		while (file.good()) {
			std::string line;
			std::getline(file, line);
			if (line.size() < 6) {
				break;
			}

			// Line should look like.
			// line = "'Marcus' 2013-05-19 2340".
			unsigned pos1 = line.find('\'');
			unsigned pos2 = line.find('\'', pos1 + 1);
			std::string name = line.substr(pos1 + 1, pos2 - pos1 - 1); // name = "Marcus".

			line = line.substr(pos2 + 1); // line = " 2013-05-19 2340".
			std::stringstream strStream(line);
			std::string date;
			int record;
			strStream >> date >> record; // date = "2013-05-19". record = 2340.

			highscore_->setNextRecord(record);
			highscore_->addNewRecord(name, date);
		}
		file.close();
	}
}

void TetrisWindow::saveHighscore() {
	std::string line;
	std::ofstream file("highscore");
	if (file.is_open() && file.good()) {
		// Line should look like.
		// line = "'Marcus' 2013-05-19 2340".
		highscore_->iterateRecords([&](int points, std::string name, std::string date) {
			file << "'" << name << "' " << date << " " << points << "\n";
		});
	}
	file.close();
}

void TetrisWindow::loadAllSettings() {
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
	}
}

void TetrisWindow::saveAllSettings() {
	std::ofstream file("settings");
	if (file.is_open() && file.good()) {
		for (const Ai& ai : activeAis_) {
			file << ai.getName() << "\n";
		}
	}
}
