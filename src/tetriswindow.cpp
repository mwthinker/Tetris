#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"
#include "computer.h"
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

	std::shared_ptr<gui::Panel> createBackgroundPanel() {
		std::shared_ptr<gui::Panel> panel = std::make_shared<gui::Panel>();
		panel->setBackground(spriteBackground.getTexture());
		panel->setLayout(std::make_shared<gui::BorderLayout>());
		return panel;
	}

	std::shared_ptr<gui::Panel> createBar() {
		std::shared_ptr<gui::Panel> bar = std::make_shared<gui::Panel>();
		bar->setPreferredSize(35, 35);
		bar->setBackgroundColor(mw::Color(.5, 0, 0, .30));
		bar->setLayout(std::make_shared<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f));
		return bar;
	}

	std::shared_ptr<gui::Panel> createPanel(float preferredWidth = 100, float preferredHeight = 100) {
		std::shared_ptr<gui::Panel> panel = std::make_shared<gui::Panel>();
		panel->setBackgroundColor(mw::Color(1, 1, 1, 0));
		panel->setPreferredSize(preferredWidth, preferredHeight);
		return panel;
	}

	std::shared_ptr<gui::Label> createLabel(std::string text, mw::Font font) {
		std::shared_ptr<gui::Label> label = std::make_shared<gui::Label>(text, font);
		label->setTextColor(mw::Color(1, 1, 1));
		label->setBackgroundColor(mw::Color(1, 1, 1, 0));
		return label;
	}

	std::shared_ptr<gui::Button> createButton(std::string text, mw::Font font) {
		std::shared_ptr<gui::Button> button = std::make_shared<gui::Button>(text, font);
		button->setFocusColor(mw::Color(.8, .1, 0, .3));
		button->setTextColor(mw::Color(1, 0.1, 0.1));
		button->setHoverColor(mw::Color(.6, .1, .1));
		button->setPushColor(mw::Color(.8, .0, 0, .7));
		button->setBackgroundColor(mw::Color(0, 0, 0, 0));
		button->setBorderColor(mw::Color(0, 0, 0, 0));
		button->setAutoSizeToFitText(true);
		return button;
	}

	void createPlayersFields(std::array<std::shared_ptr<gui::TextField>, 4>& names, std::array<std::shared_ptr<gui::Panel>, 4>&  players) {
		for (unsigned int i = 0; i < players.size(); ++i) {
			players[i] = createPanel(400, 35);
			std::stringstream stream;
			stream << "Human " << i + 1;
			players[i]->add(createLabel(stream.str(), fontDefault18));
			names[i] = std::make_shared<gui::TextField>(stream.str(), fontDefault18);
			players[i]->add(names[i]);
			players[i]->setVisible(false);
		}
	}

	void showHideHumanFields(std::shared_ptr<ManButton> humans, std::array<std::shared_ptr<gui::Panel>, 4>& humanPanels) {
		for (unsigned int i = 0; i < humanPanels.size(); ++i) {
			if (i < humans->getNbr()) {
				humanPanels[i]->setVisible(true);
			} else {
				humanPanels[i]->setVisible(false);
			}
		}
	}

}

TetrisWindow::TetrisWindow() : gui::Frame(512, 512, true, "MWetris", "images/tetris.bmp") {
	nbrOfHumanPlayers_ = 1;
	nbrOfComputerPlayers_ = 0;

	game_ = std::make_shared<GameComponent>(tetrisGame_);

	addUpdateListener([&](gui::Frame& frame, Uint32 deltaTime) {
		tetrisGame_.update(deltaTime);
	});

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
	addSdlEventListener(std::bind(&TetrisWindow::updateDevices, this, std::placeholders::_1, std::placeholders::_2));

	// Create all frames.
	getCurrentPanel()->setBackground(spriteBackground.getTexture());
	menuIndex_ = getCurrentPanelIndex();
	playIndex_ = push_back(createBackgroundPanel());
	highscoreIndex_ = push_back(createBackgroundPanel());
	customIndex_ = push_back(createBackgroundPanel());
	settingsIndex_ = push_back(createBackgroundPanel());
	newHighscoreIndex_ = push_back(createBackgroundPanel());
	networkIndex_ = push_back(createBackgroundPanel());
	createClientIndex_ = push_back(createBackgroundPanel());
	createServerIndex_ = push_back(createBackgroundPanel());
	loobyClientIndex_ = push_back(createBackgroundPanel());
	loobyServerIndex_ = push_back(createBackgroundPanel());
	waitToConnectIndex_ = push_back(createBackgroundPanel());
	networkPlayIndex_ = push_back(createBackgroundPanel());
	aiIndex_ = push_back(createBackgroundPanel());

	initMenuPanel();
	initPlayPanel();
	initHighscorePanel();
	initNewHighscorePanel();
	initCustomPlayPanel();
	initSettingsPanel();
	initCreateServerPanel();
	initCreateClientPanel();
	initServerLoobyPanel();
	initClientLoobyPanel();
	initWaitToConnectPanel();

	// Init local game settings.
	createLocalGame(TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);

	setCurrentPanel(menuIndex_);

	loadAllSettings();
	loadHighscore();
}

void TetrisWindow::updateDevices(Frame& frame, const SDL_Event& windowEvent) {
	for (DevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

TetrisWindow::~TetrisWindow() {
}

void TetrisWindow::initMenuPanel() {
	setCurrentPanel(menuIndex_);

	resume_ = createButton("Resume", fontDefault30);
	resume_->addActionListener([&](gui::Component&) {
		setCurrentPanel(playIndex_);
	});
	resume_->setVisible(false);

	auto bar = createBar();
	bar->add(resume_);

	add(bar, gui::BorderLayout::NORTH);

	auto panel = createPanel(400, 400);
	panel->setLayout(std::make_shared<gui::VerticalLayout>(5.f, 15.f, 10.f));
	panel->setBackgroundColor(mw::Color(1, 1, 1, 0));

	auto label = createLabel("MWetris", fontDefault50);
	panel->addToGroup(label);

	auto b1 = createButton("Play", fontDefault30);
	panel->addToGroup(b1);
	b1->addActionListener([&](gui::Component&) {
		createLocalGame(TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);
		setCurrentPanel(playIndex_);
		resume_->setVisible(true);
	});

	auto b2 = createButton("Custom play", fontDefault30);
	panel->addToGroup(b2);
	b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(customIndex_);
	});

	auto b3 = createButton("Network play", fontDefault30);
	panel->addToGroup(b3);
	b3->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	auto b4 = createButton("Highscore", fontDefault30);
	panel->addToGroup(b4);
	b4->addActionListener([&](gui::Component&) {
		setCurrentPanel(highscoreIndex_);
	});

	auto b5 = createButton("Settings", fontDefault30);
	b5->addActionListener([&](gui::Component&) {
		setCurrentPanel(settingsIndex_);
	});

	panel->addToGroup(b5);
	auto b6 = createButton("Exit", fontDefault30);
	b6->addActionListener([&](gui::Component&) {
		Window::quit();
	});
	panel->addToGroup(b6);

	add(panel, gui::BorderLayout::WEST);	
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

	auto bar = createBar();
	bar->setLayout(std::make_shared<gui::BorderLayout>());

	auto p1 = createPanel(300.f, 100.f);
	auto p2 = createPanel(300.f, 100.f);
	// Make panels transparent.
	p1->setLayout(std::make_shared<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f));
	p2->setLayout(std::make_shared<gui::FlowLayout>(gui::FlowLayout::RIGHT, 5.f, 0.f));

	menu_ = createButton("Menu", fontDefault30);
	menu_->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		if (tetrisGame_.getStatus() == TetrisGame::CLIENT || tetrisGame_.getStatus() == TetrisGame::SERVER) {
			tetrisGame_.closeGame();
			menu_->setLabel("Menu");
			nbrAis_->setVisible(true);
			nbrHumans_->setVisible(true);
			restart_->setVisible(true);
		}
	});
	p1->add(menu_);

	restart_ = createButton("Restart", fontDefault30);
	restart_->addActionListener([&](gui::Component&) {
		tetrisGame_.restartGame();
	});
	p1->add(restart_);
	pauseButton_ = createButton("Pause", fontDefault30);
	pauseButton_->addActionListener([&](gui::Component&) {
		tetrisGame_.pause();
	});
	p2->add(pauseButton_);

	nbrHumans_ = std::make_shared<ManButton>(devices_.size(), spriteMan, spriteCross);
	nbrHumans_->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 20, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});
	p1->add(nbrHumans_);
	nbrAis_ = std::make_shared<ManButton>(4, spriteComputer, spriteCross);
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 20, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});
	p2->add(nbrAis_);
		
	bar->add(p1, gui::BorderLayout::WEST);
	bar->add(p2, gui::BorderLayout::EAST);
	add(bar, gui::BorderLayout::NORTH);
	add(game_, gui::BorderLayout::CENTER);
	
	addKeyListener([&](gui::Component& c, const SDL_Event& keyEvent) {
		switch (keyEvent.type) {
			case SDL_KEYDOWN:
				switch (keyEvent.key.keysym.sym) {
					case SDLK_F2:
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

	auto bar = createBar();
	auto b1 = createButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	add(bar, gui::BorderLayout::NORTH);
	highscore_ = std::make_shared<Highscore>(10, mw::Color(1, 1, 1));
	add(highscore_, gui::BorderLayout::CENTER);
}

void TetrisWindow::initNewHighscorePanel() {
	setCurrentPanel(newHighscoreIndex_);
	add(createBar(), gui::BorderLayout::NORTH);

	auto panel = createPanel(200, 200);
	add(panel, gui::BorderLayout::CENTER);

	panel->add(createLabel("Name: ", fontDefault18));

	textField_ = std::make_shared<gui::TextField>(fontDefault18);
	textField_->addActionListener([&](gui::Component& c) {
		gui::TextField& textField = static_cast<gui::TextField&>(c);
		std::string name = textField.getText();

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
			setCurrentPanel(menuIndex_);
		}
	});
	panel->add(textField_);
	auto button = createButton("Ok", fontDefault18);
	button->addActionListener([&](gui::Component& c) {
		textField_->doAction();
	});
	panel->add(button);
}

void TetrisWindow::initCustomPlayPanel() {
	setCurrentPanel(customIndex_);
	auto bar = createBar();
	auto b1 = createButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	bar->add(b1);
	add(bar, gui::BorderLayout::NORTH);

	auto p1 = createPanel(450, 100);
	p1->add(createLabel("Width", fontDefault18));
	customWidthField_ = std::make_shared<gui::TextField>("10", fontDefault18);
	p1->add(customWidthField_);
	p1->add(createLabel("Height", fontDefault18));
	customHeightField_ = std::make_shared<gui::TextField>("20", fontDefault18);
	p1->add(customHeightField_);

	auto p2 = createPanel(100, 100);
	p2->add(createLabel("Min Level", fontDefault18));
	customMinLevel_ = std::make_shared<gui::TextField>("1", fontDefault18);
	p2->add(customMinLevel_);
	p2->add(createLabel("Max Level", fontDefault18));
	customMaxLevel_ = std::make_shared<gui::TextField>("20", fontDefault18);
	p2->add(customMaxLevel_);

	auto centerPanel = createPanel();
	centerPanel->setLayout(std::make_shared<gui::VerticalLayout>());

	centerPanel->add(p1);
	centerPanel->add(p2);

	auto button = createButton("Play", fontDefault30);
	centerPanel->add(button);

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initSettingsPanel() {
	setCurrentPanel(settingsIndex_);
	auto bar = createBar();
	auto b1 = createButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	bar->add(b1);
	add(bar, gui::BorderLayout::NORTH);
}

void TetrisWindow::initCreateServerPanel() {
	setCurrentPanel(createServerIndex_);
	auto bar = createBar();
	auto b1 = createButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	auto b2 = createButton("Client", fontDefault30);
	b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(createClientIndex_);
	});
	bar->add(b2);

	add(bar, gui::BorderLayout::NORTH);

	auto centerPanel = createPanel();
	centerPanel->setLayout(std::make_shared<gui::VerticalLayout>());

	auto p1 = createPanel(450, 40);
	p1->add(createLabel("Width", fontDefault18));
	serverWidthField_ = std::make_shared<gui::TextField>("10", fontDefault18);
	p1->add(serverWidthField_);
	p1->add(createLabel("Height", fontDefault18));
	serverHeightField_ = std::make_shared<gui::TextField>("20", fontDefault18);
	p1->add(serverHeightField_);
	centerPanel->add(p1);

	auto p2 = createPanel(100, 150);
	p2->add(createLabel("Min Level", fontDefault18));
	serverMinLevel_ = std::make_shared<gui::TextField>("1", fontDefault18);
	p2->add(serverMinLevel_);
	p2->add(createLabel("Max Level", fontDefault18));
	serverMaxLevel_ = std::make_shared<gui::TextField>("20", fontDefault18);
	p2->add(serverMaxLevel_);
	centerPanel->add(p2);

	auto p3 = createPanel(450, 40);
	p3->add(createLabel("Port", fontDefault18));
	portServer_ = std::make_shared<gui::TextField>("11155", fontDefault18);
	p3->add(portServer_);
	centerPanel->add(p3);

	auto p4 = createPanel(450, 40);
	p4->add(createLabel("Local players", fontDefault18));
	centerPanel->add(p4);

	nbrHumansServer_ = std::make_shared<ManButton>(devices_.size(), spriteMan, spriteCross);
	p4->add(nbrHumansServer_);
	nbrAisServer_ = std::make_shared<ManButton>(activeAis_.size(), spriteComputer, spriteCross);
	p4->add(nbrAisServer_);

	createPlayersFields(namesServer_, playersServer_);
	showHideHumanFields(nbrHumansServer_, playersServer_);

	for (std::shared_ptr<gui::Panel>& panel : playersServer_) {
		centerPanel->add(panel);
	}

	nbrAisServer_->addActionListener([&](gui::Component& c) {
		nbrOfComputerPlayers_ = nbrAisServer_->getNbr();
	});

	nbrHumansServer_->addActionListener([&](gui::Component& c) {
		for (unsigned int i = 0; i < playersServer_.size(); ++i) {
			if (i < nbrHumansServer_->getNbr()) {
				playersServer_[i]->setVisible(true);
			} else {
				playersServer_[i]->setVisible(false);
			}
		}
		nbrOfHumanPlayers_ = nbrHumansServer_->getNbr();
	});

	auto button = createButton("Connect", fontDefault30);
	centerPanel->add(button);
	button->addActionListener([&](gui::Component& c) {
		int port, width, height;
		std::stringstream stream1;
		stream1 << portServer_->getText();
		stream1 >> port;
		std::stringstream stream2;
		stream2 << serverWidthField_->getText();
		stream2 >> width;
		std::stringstream stream3;
		stream3 << serverHeightField_->getText();
		stream3 >> height;
		
		createServerGame(port, width, height);
	});

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initCreateClientPanel() {
	setCurrentPanel(createClientIndex_);
	auto bar = createBar();
	auto b1 = createButton("Menu", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	auto b2 = createButton("Client", fontDefault30);
	b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});
	bar->add(b2);

	add(bar, gui::BorderLayout::NORTH);

	auto centerPanel = createPanel();
	centerPanel->setLayout(std::make_shared<gui::VerticalLayout>());

	auto p1 = createPanel(450, 40);
	p1->add(createLabel("Ip", fontDefault18));
	ipClient_ = std::make_shared<gui::TextField>("", fontDefault18);
	p1->add(ipClient_);
	p1->add(createLabel("Port", fontDefault18));
	portClient_ = std::make_shared<gui::TextField>("11155", fontDefault18);
	p1->add(portClient_);
	centerPanel->add(p1);

	auto p2 = createPanel(450, 40);
	p2->add(createLabel("Local players", fontDefault18));
	centerPanel->add(p2);

	nbrHumansClient_ = std::make_shared<ManButton>(devices_.size(), spriteMan, spriteCross);
	p2->add(nbrHumansClient_);
	nbrAisClient_ = std::make_shared<ManButton>(activeAis_.size(), spriteComputer, spriteCross);
	nbrAisClient_->addActionListener([&](gui::Component& c) {
		nbrOfComputerPlayers_ = nbrAisServer_->getNbr();
	});

	p2->add(nbrAisClient_);

	createPlayersFields(namesClient_, playersClient_);

	showHideHumanFields(nbrHumansClient_, playersClient_);
	for (std::shared_ptr<gui::Panel>& panel : playersClient_) {
		centerPanel->add(panel);
	}

	nbrHumansClient_->addActionListener([&](gui::Component& c) {
		for (unsigned int i = 0; i < playersClient_.size(); ++i) {
			if (i < nbrHumansClient_->getNbr()) {
				playersClient_[i]->setVisible(true);
			} else {
				playersClient_[i]->setVisible(false);
			}
		}
		nbrOfHumanPlayers_ = nbrHumansServer_->getNbr();
	});

	auto button = createButton("Connect", fontDefault30);
	button->addActionListener([&](gui::Component& c) {
		int port;
		std::stringstream stream1;
		stream1 << portClient_->getText();
		stream1 >> port;

		createClientGame(port, ipClient_->getText());
	});

	centerPanel->add(button);

	add(centerPanel, gui::BorderLayout::CENTER);
}

void TetrisWindow::initServerLoobyPanel() {
	setCurrentPanel(loobyServerIndex_);
	auto bar = createBar();
	auto b1 = createButton("Abort", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});
	bar->add(b1);

	serverLooby_ = std::make_shared<NetworkLooby>();

	add(bar, gui::BorderLayout::NORTH);
	add(serverLooby_, gui::BorderLayout::CENTER);
	auto p = createPanel();
	auto b2 = createButton("Ready", fontDefault30);
	p->add(b2);
	b2->addActionListener([&](gui::Component&) {
		tetrisGame_.changeReadyState();
	});
	auto b3 = createButton("Start", fontDefault30);
	b3->addActionListener([&](gui::Component&) {
		tetrisGame_.startGame();
	});
	p->add(b3);
	add(p, gui::BorderLayout::SOUTH);
}

void TetrisWindow::initClientLoobyPanel() {
	setCurrentPanel(loobyClientIndex_);
	auto bar = createBar();
	auto b1 = createButton("Abort", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});

	auto b2 = createButton("Ready", fontDefault30);
	b2->addActionListener([&](gui::Component&) {
		tetrisGame_.changeReadyState();
	});

	clientLooby_ = std::make_shared<NetworkLooby>();

	add(bar, gui::BorderLayout::NORTH);
	add(clientLooby_, gui::BorderLayout::CENTER);

	auto p = createPanel();
	p->add(b2);
	add(p, gui::BorderLayout::SOUTH);
}

void TetrisWindow::initWaitToConnectPanel() {
	setCurrentPanel(waitToConnectIndex_);
	auto bar = createBar();
	auto b1 = createButton("Abort", fontDefault30);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
	bar->add(b1);

	add(bar, gui::BorderLayout::NORTH);
	add(createLabel("Waiting for the server to accept connection!", fontDefault18), gui::BorderLayout::CENTER);
}

void TetrisWindow::createLocalGame(int width, int height, int maxLevel) {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	for (DevicePtr& device : devices_) {
		device->setPlayerName(device->getName());
	}

	tetrisGame_.closeGame();
	tetrisGame_.setAis(activeAis_[0], activeAis_[1], activeAis_[2], activeAis_[3]);
	tetrisGame_.createLocalGame(tmpDevices, nbrOfComputerPlayers_, width, height, maxLevel);
	tetrisGame_.startGame();
}

void TetrisWindow::createServerGame(int port, int width, int height) {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < nbrOfHumanPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
		devices_[i]->setPlayerName(namesServer_[i]->getText());
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
		devices_[i]->setPlayerName(namesClient_[i]->getText());
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
		} else {
			pauseButton_->setLabel("Pause");
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
				menu_->setLabel("Abort");
				nbrAis_->setVisible(false);
				nbrHumans_->setVisible(false);
				restart_->setVisible(false);
				resume_->setVisible(false);
				break;
			case GameStart::SERVER:
				menu_->setLabel("Abort");
				nbrAis_->setVisible(false);
				nbrHumans_->setVisible(false);
				resume_->setVisible(false);
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
