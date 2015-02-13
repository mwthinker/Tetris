#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"
#include "computer.h"
#include "manbutton.h"
#include "highscore.h"
#include "gamecomponent.h"
#include "joystick.h"
#include "networklooby.h"
#include "tetrisentry.h"

#include "guiclasses.h"
#include <gui/borderlayout.h>
#include <gui/flowlayout.h>
#include <gui/verticallayout.h>
#include <gui/gridlayout.h>

#include <mw/sprite.h>
#include <mw/color.h>
#include <mw/joystick.h>

#include <ctime>
#include <iostream>
#include <sstream>

namespace {

    void showHideHumanFields(std::shared_ptr<ManButton> humans, std::array<std::shared_ptr<gui::Panel>, 4>& humanPanels) {
		for (unsigned int i = 0; i < humanPanels.size(); ++i) {
			if (i < humans->getNbr()) {
				humanPanels[i]->setVisible(true);
			} else {
				humanPanels[i]->setVisible(false);
			}
		}
	}

} // Namespace anonymous.

TetrisWindow::TetrisWindow(TetrisEntry e, int frame) : tetrisEntry_(e),
	gui::Frame(e.getDeepChildEntry("window positionX").getInt(),
        e.getDeepChildEntry("window positionY").getInt(),
        e.getDeepChildEntry("window width").getInt(),
        e.getDeepChildEntry("window height").getInt(),
        true,
        "MWetris",
        e.getDeepChildEntry("window icon").getString(),
        !e.getDeepChildEntry("window border").getBool()),

	windowFollowMouse_(false), followMouseX_(0), followMouseY_(0),
	nbrOfHumanPlayers_(1), nbrOfComputerPlayers_(0) {

	SDL_SetWindowMinimumSize(mw::Window::getSdlWindow(),
		tetrisEntry_.getDeepChildEntry("window minWidth").getInt(),
		tetrisEntry_.getDeepChildEntry("window minHeight").getInt());

	if (tetrisEntry_.getDeepChildEntry("window maximized").getBool()) {
		SDL_MaximizeWindow(mw::Window::getSdlWindow());
	}

	SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_);
	SDL_GetWindowSize(getSdlWindow(), &lastWidth_, &lastHeight_);

	game_ = std::make_shared<GameComponent>(tetrisGame_, tetrisEntry_);

	addUpdateListener([&](gui::Frame& frame, Uint32 deltaTime) {
		SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_); // Update last window position.
		tetrisGame_.update(deltaTime);
	});

	// Saves the last window position. And makes the window movable by holding down left mouse button.
	addSdlEventListener(std::bind(&TetrisWindow::sdlEventListener, this, std::placeholders::_1, std::placeholders::_2));

	tetrisGame_.addCallback([&](NetworkEventPtr nEvent) {
		handleConnectionEvent(nEvent);
	});

	// Initializes default keyboard devices for two players.
	DevicePtr device1(new Keyboard("Keyboard 1", SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP));
	devices_.push_back(device1);
	DevicePtr device2(new Keyboard("Keyboard 2", SDLK_s, SDLK_a, SDLK_d, SDLK_w));
	devices_.push_back(device2);

	setDefaultClosing(true);

	// Initialization of all joysticks!
	const std::vector<mw::JoystickPtr>& joystics = mw::Joystick::getJoystics();
	for (const mw::JoystickPtr& joystick : joystics) {
		std::cout << joystick->getName() << std::endl;
		DevicePtr device(new Joystick(joystick, 0, 1));
		devices_.push_back(device);
	}
	addSdlEventListener(std::bind(&TetrisWindow::updateDevices, this, std::placeholders::_1, std::placeholders::_2));

	// Create all frames.
	auto background = tetrisEntry_.getDeepChildEntry("window sprites background").getSprite();
	getCurrentPanel()->setBackground(background);
	menuIndex_ = getCurrentPanelIndex();
	playIndex_ = pushBackPanel(std::make_shared<Background>(background));
	highscoreIndex_ = pushBackPanel(std::make_shared<Background>(background));
	customIndex_ = pushBackPanel(std::make_shared<Background>(background));
	settingsIndex_ = pushBackPanel(std::make_shared<Background>(background));
	newHighscoreIndex_ = pushBackPanel(std::make_shared<Background>(background));
	networkIndex_ = pushBackPanel(std::make_shared<Background>(background));
	createClientIndex_ = pushBackPanel(std::make_shared<Background>(background));
	createServerIndex_ = pushBackPanel(std::make_shared<Background>(background));
	loobyClientIndex_ = pushBackPanel(std::make_shared<Background>(background));
	loobyServerIndex_ = pushBackPanel(std::make_shared<Background>(background));
	waitToConnectIndex_ = pushBackPanel(std::make_shared<Background>(background));
	networkPlayIndex_ = pushBackPanel(std::make_shared<Background>(background));
	aiIndex_ = pushBackPanel(std::make_shared<Background>(background));

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

	if (frame >= 0 && frame < aiIndex_) {
		if (frame == playIndex_) {
			// Initialization local game settings.
			createLocalGame(TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);
		}
		setCurrentPanel(frame);
	}

	loadHighscore();

	auto aiEntry = tetrisEntry_.getDeepChildEntry("players ais player");
	ais_.clear();
	while (aiEntry.hasData()) {
		ais_.push_back(aiEntry.getAi());
		aiEntry = aiEntry.getSibling("player");
	}
}

mw::Font TetrisWindow::getDefaultFont(int size) {
	return tetrisEntry_.getDeepChildEntry("window font").getFont(size);
}

void TetrisWindow::updateDevices(gui::Frame& frame, const SDL_Event& windowEvent) {
	for (DevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

TetrisWindow::~TetrisWindow() {
	tetrisEntry_.getDeepChildEntry("window positionX").setInt(lastX_);
	tetrisEntry_.getDeepChildEntry("window positionY").setInt(lastY_);
	tetrisEntry_.getDeepChildEntry("window width").setInt(lastWidth_);
	tetrisEntry_.getDeepChildEntry("window height").setInt(lastHeight_);
	tetrisEntry_.save();
}

void TetrisWindow::createPlayersFields(const mw::Font& font, std::array<std::shared_ptr<gui::TextField>, 4>& names, std::array<std::shared_ptr<gui::Panel>, 4>&  players) const {
	for (unsigned int i = 0; i < players.size(); ++i) {
		players[i] = std::make_shared<TransparentPanel>(400.f, 35.f);
		std::stringstream stream;
		stream << "Human " << i + 1;
		players[i]->addDefault<Label>(stream.str(), font, tetrisEntry_);
		names[i] = std::make_shared<TextField>(stream.str(), font);
		players[i]->addDefault(names[i]);
		players[i]->setVisible(false);
	}
}

void TetrisWindow::initMenuPanel() {
	setCurrentPanel(menuIndex_);

	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	resume_ = bar->addDefault<Button>("Resume", getDefaultFont(30), tetrisEntry_);
    resume_->setVisible(false);
    resume_->addActionListener([&](gui::Component&) {
		setCurrentPanel(playIndex_);
	});

	// 400 in order to be wide enough for all buttons.
	auto panel = add<TransparentPanel>(gui::BorderLayout::WEST, 400.f);

	panel->setLayout<gui::VerticalLayout>(5.f, 15.f, 10.f);
	panel->setBackgroundColor(1, 1, 1, 0);
	panel->addDefault<Label>("MWetris", getDefaultFont(50), tetrisEntry_);

	auto b1 = panel->addDefaultToGroup<Button>("Play", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		createLocalGame(TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);
		setCurrentPanel(playIndex_);
		resume_->setVisible(true);
	});

	auto b2 = panel->addDefaultToGroup<Button>("Custom play", getDefaultFont(30), tetrisEntry_);
	b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(customIndex_);
	});

	auto b3 = panel->addDefaultToGroup<Button>("Network play", getDefaultFont(30), tetrisEntry_);
	b3->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	auto b4 = panel->addDefaultToGroup<Button>("Highscore", getDefaultFont(30), tetrisEntry_);
	b4->addActionListener([&](gui::Component&) {
		setCurrentPanel(highscoreIndex_);
	});

	auto b5 = panel->addDefaultToGroup<Button>("Settings", getDefaultFont(30), tetrisEntry_);
	b5->addActionListener([&](gui::Component&) {
		setCurrentPanel(settingsIndex_);
	});

	auto b6 = panel->addDefaultToGroup<Button>("Exit", getDefaultFont(30), tetrisEntry_);
	b6->addActionListener([&](gui::Component&) {
		Window::quit();
	});
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

    auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	bar->setLayout<gui::BorderLayout>();

	auto p1 = bar->add<TransparentPanel>(gui::BorderLayout::WEST, 300.f, 100.f);
	auto p2 = bar->add<TransparentPanel>(gui::BorderLayout::EAST, 300.f, 100.f);

	p1->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
	p2->setLayout<gui::FlowLayout>(gui::FlowLayout::RIGHT, 5.f, 0.f);

	menu_ = p1->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
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

	restart_ = p1->addDefault<Button>("Restart", getDefaultFont(30), tetrisEntry_);
	restart_->addActionListener([&](gui::Component&) {
		tetrisGame_.restartGame();
	});

	pauseButton_ = p2->addDefault<Button>("Pause", getDefaultFont(30), tetrisEntry_);
	pauseButton_->addActionListener([&](gui::Component&) {
		tetrisGame_.pause();
	});

	nbrHumans_ = p1->addDefault<ManButton>(devices_.size(), tetrisEntry_.getDeepChildEntry("window sprites human").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
	nbrHumans_->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 24, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});

	nbrAis_ = p2->addDefault<ManButton>(4, tetrisEntry_.getDeepChildEntry("window sprites computer").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.createLocalGame(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()), nbrAis_->getNbr(), 10, 24, 20);
		tetrisGame_.startGame();
		tetrisGame_.restartGame();
	});

    // Add the game component, already created in the constructor.
	add(gui::BorderLayout::CENTER, game_);

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

	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	auto b1 = bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    // The component is reused in callbacks.
	highscore_ = add<Highscore>(gui::BorderLayout::CENTER, 10, mw::Color(1, 1, 1), getDefaultFont(18));
}

void TetrisWindow::initNewHighscorePanel() {
	setCurrentPanel(newHighscoreIndex_);
	add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto panel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	panel->addDefault<Label>("Name: ", getDefaultFont(18), tetrisEntry_);

    textField_ = panel->addDefault<TextField>(getDefaultFont(18));
	textField_->addActionListener([&](gui::Component& c) {
		TextField& textField = static_cast<TextField&>(c);
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
			saveHighscore();
		}
	});

	auto button = panel->addDefault<Button>("Ok", getDefaultFont(18), tetrisEntry_);
	button->addActionListener([&](gui::Component& c) {
		textField_->doAction();
	});
}

void TetrisWindow::initCustomPlayPanel() {
	setCurrentPanel(customIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 100.f);
	p1->addDefault<Label>("Width", getDefaultFont(18), tetrisEntry_);
	customWidthField_  = p1->addDefault<TextField>("10", getDefaultFont(18));
	p1->addDefault<Label>("Height", getDefaultFont(18), tetrisEntry_);
	customHeightField_ = p1->addDefault<TextField>("24", getDefaultFont(18));

	auto p2 = centerPanel->addDefault<TransparentPanel>(100.f, 100.f);
	p2->addDefault<Label>("Min Level", getDefaultFont(18), tetrisEntry_);
	customMinLevel_ = p2->addDefault<TextField>("1", getDefaultFont(18));
	p2->addDefault<Label>("Max Level", getDefaultFont(18), tetrisEntry_);
	customMaxLevel_ = p2->addDefault<TextField>("24", getDefaultFont(18));

	centerPanel->addDefault<Button>("Play", getDefaultFont(30), tetrisEntry_);
}

void TetrisWindow::initSettingsPanel() {
	setCurrentPanel(settingsIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    auto p = add<TransparentPanel>(gui::BorderLayout::CENTER);
    p->setLayout<gui::VerticalLayout>();
	p->addDefault<Label>("Settings", getDefaultFont(30), tetrisEntry_);

	auto checkBox1 = p->addDefault<CheckBox>("Border around window", getDefaultFont(18), tetrisEntry_);
	checkBox1->setSelected(tetrisEntry_.getDeepChildEntry("window border").getBool());
	checkBox1->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		tetrisEntry_.getDeepChildEntry("window border").setBool(check.isSelected());
		tetrisEntry_.save();
	});
	auto checkBox2 = p->addDefault<CheckBox>("Fullscreen on double click", getDefaultFont(18), tetrisEntry_);
	checkBox2->setSelected(tetrisEntry_.getDeepChildEntry("window fullscreenOnDoubleClick").getBool());
	checkBox2->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		tetrisEntry_.getDeepChildEntry("window fullscreenOnDoubleClick").setBool(check.isSelected());
		tetrisEntry_.save();
	});

	auto checkBox3 = p->addDefault<CheckBox>("Move the window by holding down left mouse button", getDefaultFont(18), tetrisEntry_);
	checkBox3->setSelected(tetrisEntry_.getDeepChildEntry("window moveWindowByHoldingDownMouse").getBool());
	checkBox3->addActionListener([&](gui::Component& c) {
        auto& check =  static_cast<CheckBox&>(c);
		tetrisEntry_.getDeepChildEntry("window moveWindowByHoldingDownMouse").setBool(check.isSelected());
		tetrisEntry_.save();
	});
}

void TetrisWindow::initCreateServerPanel() {
	setCurrentPanel(createServerIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	auto b2 = bar->addDefault<Button>("Client", getDefaultFont(30), tetrisEntry_);
    b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(createClientIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p1->addDefault<Label>("Width", getDefaultFont(18), tetrisEntry_);
	serverWidthField_ = p1->addDefault<TextField>("10", getDefaultFont(18));
	p1->addDefault<Label>("Height", getDefaultFont(18), tetrisEntry_);
	serverHeightField_ = p1->addDefault<TextField>("24", getDefaultFont(18));

	auto p2 = centerPanel->addDefault<TransparentPanel>(100.f, 150.f);
	p2->addDefault<Label>("Min Level", getDefaultFont(18), tetrisEntry_);
	serverMinLevel_ = p2->addDefault<TextField>("1", getDefaultFont(18));
	p2->addDefault<Label>("Max Level", getDefaultFont(18), tetrisEntry_);
	serverMaxLevel_ = p2->addDefault<TextField>("20", getDefaultFont(18));

	auto p3 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p3->addDefault<Label>("Port", getDefaultFont(18), tetrisEntry_);
	portServer_ = p3->addDefault<TextField>("11155", getDefaultFont(18));

	auto p4 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p4->addDefault<Label>("Local players", getDefaultFont(18), tetrisEntry_);

	nbrHumansServer_ = p4->addDefault<ManButton>(devices_.size(), tetrisEntry_.getDeepChildEntry("window sprites human").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
	nbrAisServer_ = p4->addDefault<ManButton>(activeAis_.size(), tetrisEntry_.getDeepChildEntry("window sprites computer").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());

	createPlayersFields(getDefaultFont(18), namesServer_, playersServer_);
	showHideHumanFields(nbrHumansServer_, playersServer_);

	for (std::shared_ptr<gui::Panel>& panel : playersServer_) {
		centerPanel->addDefault(panel);
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

	auto b3 = centerPanel->addDefault<Button>("Connect", getDefaultFont(30), tetrisEntry_);
	b3->addActionListener([&](gui::Component& c) {
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
}

void TetrisWindow::initCreateClientPanel() {
	setCurrentPanel(createClientIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
    b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	auto b2 = bar->addDefault<Button>("Client", getDefaultFont(30), tetrisEntry_);
    b2->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p1->addDefault<Label>("Ip", getDefaultFont(18), tetrisEntry_);
	ipClient_ = p1->addDefault<TextField>("", getDefaultFont(18));
	p1->addDefault<Label>("Port", getDefaultFont(18), tetrisEntry_);
	portClient_ = p1->addDefault<TextField>("11155", getDefaultFont(18));

	auto p2 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p2->addDefault<Label>("Local players", getDefaultFont(18), tetrisEntry_);

	nbrHumansClient_ = p2->addDefault<ManButton>(devices_.size(), tetrisEntry_.getDeepChildEntry("window sprites human").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());

    nbrAisClient_ = p2->addDefault<ManButton>(activeAis_.size(), tetrisEntry_.getDeepChildEntry("window sprites computer").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
    nbrAisClient_->addActionListener([&](gui::Component& c) {
		nbrOfComputerPlayers_ = nbrAisServer_->getNbr();
	});

	createPlayersFields(getDefaultFont(18), namesClient_, playersClient_);

	showHideHumanFields(nbrHumansClient_, playersClient_);
	for (auto& panel : playersClient_) {
		centerPanel->addDefault(panel);
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

	auto b3 = centerPanel->addDefault<Button>("Connect", getDefaultFont(30), tetrisEntry_);
	b3->addActionListener([&](gui::Component& c) {
		int port;
		std::stringstream stream1;
		stream1 << portClient_->getText();
		stream1 >> port;

		createClientGame(port, ipClient_->getText());
	});
}

void TetrisWindow::initServerLoobyPanel() {
	setCurrentPanel(loobyServerIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Abort", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});

	serverLooby_ = add<NetworkLooby>(gui::BorderLayout::CENTER, getDefaultFont(18));
	auto p = add<TransparentPanel>(gui::BorderLayout::SOUTH);

	auto b2 = p->addDefault<Button>("Ready", getDefaultFont(30), tetrisEntry_);
	b2->addActionListener([&](gui::Component&) {
		tetrisGame_.changeReadyState();
	});

	auto b3 = p->addDefault<Button>("Start", getDefaultFont(30), tetrisEntry_);
	b3->addActionListener([&](gui::Component&) {
		tetrisGame_.startGame();
	});
}

void TetrisWindow::initClientLoobyPanel() {
	setCurrentPanel(loobyClientIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	auto b1 = std::make_shared<Button>("Abort", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});

	auto p = add<TransparentPanel>(gui::BorderLayout::SOUTH);

	auto b2 = p->addDefault<Button>("Ready", getDefaultFont(30), tetrisEntry_);
	b2->addActionListener([&](gui::Component&) {
		tetrisGame_.changeReadyState();
	});

     // The component is reused in callbacks.
	clientLooby_ = add<NetworkLooby>(gui::BorderLayout::CENTER, getDefaultFont(18));
}

void TetrisWindow::initWaitToConnectPanel() {
	setCurrentPanel(waitToConnectIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	auto b1 = bar->addDefault<Button>("Abort", getDefaultFont(30), tetrisEntry_);
	b1->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	add<Label>(gui::BorderLayout::CENTER, "Waiting for the server to accept connection!", getDefaultFont(18), tetrisEntry_);
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
	if (auto gameOver = std::dynamic_pointer_cast<GameOver>(nEvent)) {
		// Points high enough to be saved in the highscore list?
		if (highscore_->isNewRecord(gameOver->points_)) {
			// Set points in order for highscore to know which point to save in list.
			highscore_->setNextRecord(gameOver->points_);
			// In order for the user to insert name.
			setCurrentPanel(newHighscoreIndex_);
		}
	} else if (auto gameOver = std::dynamic_pointer_cast<GamePause>(nEvent)) {
		if (gameOver->pause_) {
			pauseButton_->setLabel("Unpause");
		} else {
			pauseButton_->setLabel("Pause");
		}
	} else if (auto newConnection = std::dynamic_pointer_cast<NewConnection>(nEvent)) {
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
	} else if (auto start = std::dynamic_pointer_cast<GameStart>(nEvent)) {
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
	} else if (auto ready = std::dynamic_pointer_cast<GameReady>(nEvent)) {
		serverLooby_->setReady(ready->id_, ready->ready_);
		clientLooby_->setReady(ready->id_, ready->ready_);
	}
}

void TetrisWindow::loadHighscore() {
	auto entry = tetrisEntry_.getDeepChildEntry("highscore item");
	while (entry.hasData()) {
		highscore_->setNextRecord(entry.getChildEntry("points").getInt());
		highscore_->addNewRecord(entry.getChildEntry("name").getString(), entry.getChildEntry("date").getString());
		entry = entry.getSibling("item");
	}
}

void TetrisWindow::saveHighscore() {
	auto entry = tetrisEntry_.getDeepChildEntry("highscore item");
	for (const auto& score : *highscore_) {
		entry.getChildEntry("name").setString(score.name_.getText());
		entry.getChildEntry("points").setInt(score.intPoints_);
		entry.getChildEntry("date").setString(score.date_.getText());
		entry = entry.getSibling("item");
	}
	entry.save();
}

void TetrisWindow::sdlEventListener(gui::Frame& frame, const SDL_Event& e) {
	switch (e.type) {
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save size!
						lastWidth_ = e.window.data1;
						lastHeight_ = e.window.data2;
					}
					break;
				case SDL_WINDOWEVENT_MOVED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save position!
						int x, y;
						SDL_GetWindowPosition(mw::Window::getSdlWindow(), &x, &y);
						lastX_ = x;
						lastY_ = y;
					}
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					tetrisEntry_.getDeepChildEntry("window maximized").setBool(true);
					break;
				case SDL_WINDOWEVENT_RESTORED:
					tetrisEntry_.getDeepChildEntry("window maximized").setBool(false);
					break;
			}
			break;
		case SDL_MOUSEMOTION:
			if (windowFollowMouse_ && tetrisEntry_.getDeepChildEntry("window moveWindowByHoldingDownMouse").getBool()) {
				auto w = TetrisWindow::getSdlWindow();
				SDL_SetWindowPosition(w, lastX_ + e.motion.x - followMouseX_,
					lastY_ + e.motion.y - followMouseY_);
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT) {
				windowFollowMouse_ = true;
				followMouseX_ = e.button.x;
				followMouseY_ = e.button.y;
				if (e.button.clicks == 2 && tetrisEntry_.getDeepChildEntry("window fullscreenOnDoubleClick").getBool()) {
					TetrisWindow::setFullScreen(!TetrisWindow::isFullScreen());
					windowFollowMouse_ = false;
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			windowFollowMouse_ = false;
			break;
	}
}
