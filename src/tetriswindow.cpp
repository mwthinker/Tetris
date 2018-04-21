#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "tetrisgame.h"
#include "gamecontroller.h"
#include "keyboard.h"
#include "computer.h"
#include "manbutton.h"
#include "highscore.h"
#include "gamecomponent.h"
#include "guiclasses.h"
#include "tetrisgameevent.h"
#include "tetrisdata.h"

#include <gui/borderlayout.h>
#include <gui/flowlayout.h>
#include <gui/verticallayout.h>
#include <gui/gridlayout.h>

#include <mw/sprite.h>
#include <mw/color.h>

#include <ctime>
#include <iostream>
#include <sstream>

namespace {

	std::string getCurrentDate() {
		std::time_t t = std::time(0);
		char mbstr[11];
		std::strftime(mbstr, 11, "%Y-%m-%d", std::localtime(&t));
		return mbstr;
	}

}

TetrisWindow::TetrisWindow() : 
	windowFollowMouse_(false), followMouseX_(0), followMouseY_(0),
	nbrOfHumanPlayers_(1), nbrOfComputerPlayers_(0), startFrame_(StartFrame::MENU),
	lastTimerId_(0) {

	Frame::setPosition(TetrisData::getInstance().getWindowPositionX(), TetrisData::getInstance().getWindowPositionY());
	Frame::setWindowSize(TetrisData::getInstance().getWindowWidth(), TetrisData::getInstance().getWindowHeight());
	Frame::setResizeable(TetrisData::getInstance().getWindowWidth());
	Frame::setTitle("MWetris");
	Frame::setIcon(TetrisData::getInstance().getWindowIcon());
	Frame::setBordered(TetrisData::getInstance().isWindowBordered());
	Frame::setDefaultClosing(true);
}

void TetrisWindow::initOpenGl() {
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, TetrisData::getInstance().getMultiSampleBuffers());
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, TetrisData::getInstance().getMultiSampleSamples());
	Window::setOpenGlVersion(2, 1);
	Frame::initOpenGl();
}

void TetrisWindow::initPreLoop() {
	Frame::initPreLoop();
	
	SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_);

	SDL_SetWindowMinimumSize(mw::Window::getSdlWindow(),
		TetrisData::getInstance().getWindowMinWidth(),
		TetrisData::getInstance().getWindowMinHeight());

	if (TetrisData::getInstance().isWindowMaximized()) {
		SDL_MaximizeWindow(mw::Window::getSdlWindow());
	}
	
	SDL_GL_SetSwapInterval(TetrisData::getInstance().isWindowVsync() ? 1 : 0);

	// Initializes default keyboard devices for two players.
	devices_.push_back(std::make_shared<Keyboard>("Keyboard 1", SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP, SDLK_RCTRL));
	devices_.push_back(std::make_shared<Keyboard>("Keyboard 2", SDLK_s, SDLK_a, SDLK_d, SDLK_w, SDLK_LCTRL));

	// Initialization of all joysticks!
	mw::GameController::loadAddGameControllerMappings("gamecontrollerdb.txt");	

	tetrisGame_.addCallback(std::bind(&TetrisWindow::handleConnectionEvent, this, std::placeholders::_1));

	// Create all frames.
	auto background = TetrisData::getInstance().getBackgroundSprite();
	getCurrentPanel()->setBackground(background);
	menuIndex_ = getCurrentPanelIndex();
	playIndex_ = pushBackPanel(std::make_shared<Background>(background));
	highscoreIndex_ = pushBackPanel(std::make_shared<Background>(background));
	customIndex_ = pushBackPanel(std::make_shared<Background>(background));
	settingsIndex_ = pushBackPanel(std::make_shared<Background>(background));
	newHighscoreIndex_ = pushBackPanel(std::make_shared<Background>(background));
	networkIndex_ = pushBackPanel(std::make_shared<Background>(background));

	initMenuPanel();
	initPlayPanel();
	initHighscorePanel();
	initNewHighscorePanel();
	initCustomPlayPanel();
	initSettingsPanel();
	initNetworkPanel();

	loadHighscore();
	// Init ai players.
	activeAis_[0] = findAiDevice(TetrisData::getInstance().getAi1Name());
	activeAis_[1] = findAiDevice(TetrisData::getInstance().getAi2Name());
	activeAis_[2] = findAiDevice(TetrisData::getInstance().getAi3Name());
	activeAis_[3] = findAiDevice(TetrisData::getInstance().getAi4Name());

	// Choose start panel.
	switch (startFrame_) {
		case StartFrame::MENU:
			setCurrentPanel(menuIndex_);
			break;
		case StartFrame::SERVER:
			setCurrentPanel(networkIndex_);
			radioButtonServer_->doAction();
			networkConnect_->doAction();
			break;
		case StartFrame::CLIENT:
			setCurrentPanel(networkIndex_);
			radioButtonClient_->doAction();
			networkConnect_->doAction();
			break;
		case StartFrame::LOCAL_GAME:
			// Initialization local game settings.
			tetrisGame_.createLocalGame();
			setCurrentPanel(playIndex_);
			break;
	}
}

void TetrisWindow::update(double deltaTime) {
	if (playIndex_ == getCurrentPanelIndex() ||
		networkIndex_ == getCurrentPanelIndex()) {
		
		SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_); // Update last window position.
		tetrisGame_.update(deltaTime);
	}
	gui::Frame::update(deltaTime);
}

void TetrisWindow::eventUpdate(const SDL_Event& windowEvent) {
	gui::Frame::eventUpdate(windowEvent);
	
	for (SdlDevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}

	// Saves the last window position. And makes the window movable by holding down left mouse button.
	sdlEventListener(windowEvent);
}

void TetrisWindow::resumeGame() {
	int rows = TetrisData::getInstance().getActiveLocalGameRows();
	int columns = TetrisData::getInstance().getActiveLocalGameColumns();

	int ais = 0;
	int humans = 0;

	std::vector<PlayerData> playerDataVector = TetrisData::getInstance().getActiveLocalGamePlayers();
	for (PlayerData& playerData : playerDataVector) {
		if (playerData.ai_) {
			playerData.device_ = findAiDevice(playerData.deviceName_);
			++ais;
		} else {
			playerData.device_ = findHumanDevice(playerData.deviceName_);
			++humans;
		}
	}
	tetrisGame_.resumeGame(columns, rows, playerDataVector);
	nbrAis_->setNbr(ais);
	nbrHumans_->setNbr(humans);
}

void TetrisWindow::saveCurrentLocalGame() {
	if (tetrisGame_.isDefaultGame() && getCurrentPanelIndex() == playIndex_
		&& tetrisGame_.getStatus() == TetrisGame::LOCAL) {

		// Save only when the game is active and is a local default game.
		TetrisData::getInstance().setActiveLocalGame(tetrisGame_.getColumns(), 
			tetrisGame_.getRows(), tetrisGame_.getPlayerData());
		TetrisData::getInstance().save();
	}
}

void TetrisWindow::startServerLoop(int port) {
	startFrame_ = StartFrame::SERVER;
	TetrisData::getInstance().setPort(port);
	startLoop();
}

void TetrisWindow::startClientLoop(int port, std::string ip) {
	startFrame_ = StartFrame::CLIENT;
	TetrisData::getInstance().setPort(port);
	TetrisData::getInstance().setIp(ip);
	startLoop();
}

TetrisWindow::~TetrisWindow() {
	TetrisData::getInstance().save();
}

void TetrisWindow::initMenuPanel() {
	setCurrentPanel(menuIndex_);
	
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	// 400 in order to be wide enough for all buttons.
	auto panel = add<TransparentPanel>(gui::BorderLayout::WEST, 400.f);

	panel->setLayout<gui::VerticalLayout>(5.f, 15.f, 10.f);
	panel->setBackgroundColor(1, 1, 1, 0);
	panel->addDefault<Label>("MWetris", TetrisData::getInstance().getDefaultFont(50));
	
	auto button = panel->addDefault<Button>("Play", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		resumeGame();
		if (tetrisGame_.getNbrOfPlayers() == 1) {
			tetrisGame_.pause();
		}
		setCurrentPanel(playIndex_);
	});
	groupMenu_.add(button);

	button = panel->addDefault<Button>("Custom play", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		setCurrentPanel(customIndex_);
	});
	groupMenu_.add(button);

	button = panel->addDefault<Button>("Network play", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		setCurrentPanel(networkIndex_);
	});
	groupMenu_.add(button);

	button = panel->addDefault<Button>("Highscore", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		setCurrentPanel(highscoreIndex_);
	});
	groupMenu_.add(button);

	button = panel->addDefault<Button>("Settings", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		setCurrentPanel(settingsIndex_);
	});
	groupMenu_.add(button);
	
	button = panel->addDefault<Button>("Exit", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		Window::quit();
	});
	groupMenu_.add(button);

	// In order for the user to traverse the menu with the keyboard.
	groupMenu_.setVerticalArrows(true);
	addKeyListener([&](gui::Component&, const SDL_Event& sdlEvent) {
		groupMenu_.handleKeyboard(sdlEvent);
	});
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

    auto bar = add<Bar>(gui::BorderLayout::NORTH);
	bar->setLayout<gui::BorderLayout>();

	manBar_ = bar->add<TransparentPanel>(gui::BorderLayout::WEST, 400.f, 100.f);
	auto p2 = bar->add<TransparentPanel>(gui::BorderLayout::EAST, 200.f, 100.f);

	manBar_->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
	p2->setLayout<gui::FlowLayout>(gui::FlowLayout::RIGHT, 5.f, 0.f);
	
	menu_ = manBar_->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30));
	menu_->addActionListener([&](gui::Component&) {
		saveCurrentLocalGame(); // Must be called first, must be in play frame.
		setCurrentPanel(menuIndex_);
		if (tetrisGame_.getStatus() == TetrisGame::CLIENT || tetrisGame_.getStatus() == TetrisGame::SERVER) {
			tetrisGame_.closeGame();
			menu_->setLabel("Menu");
			setTitle("MWetris");
		}
	});
	menu_->setFocus(false);

	restart_ = manBar_->addDefault<Button>("Restart", TetrisData::getInstance().getDefaultFont(30));
	restart_->addActionListener([&](gui::Component&) {
		if (tetrisGame_.getStatus() == TetrisGame::CLIENT || tetrisGame_.getStatus() == TetrisGame::SERVER) {
			tetrisGame_.restartGame();
		} else {
			tetrisGame_.closeGame();
			setPlayers(tetrisGame_.getColumns(), tetrisGame_.getRows());
			tetrisGame_.createLocalGame(tetrisGame_.getColumns(), tetrisGame_.getRows());
		}
	});

	nbrHumans_ = manBar_->addDefault<ManButton>(devices_.size(), TetrisData::getInstance().getHumanSprite(), TetrisData::getInstance().getCrossSprite());
	nbrHumans_->addActionListener([&](gui::Component&) {
		setPlayers(tetrisGame_.getColumns(), tetrisGame_.getRows());
	});

	nbrAis_ = manBar_->addDefault<ManButton>(activeAis_.size(), TetrisData::getInstance().getComputerSprite(), TetrisData::getInstance().getCrossSprite());
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component&) {
		setPlayers(tetrisGame_.getColumns(), tetrisGame_.getRows());
	});

	pauseButton_ = p2->addDefault<Button>("Pause", TetrisData::getInstance().getDefaultFont(30));
	pauseButton_->addActionListener([&](gui::Component&) {
		tetrisGame_.pause();
	});
	
    // Add the game component, already created in the constructor.
	game_ = std::make_shared<GameComponent>(tetrisGame_);
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

	addPanelChangeListener([&](gui::Component& c, bool enterFrame) {
		menu_->setFocus(false);
		panelChangeListenerFpsLimiter(c, enterFrame);
	});
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initHighscorePanel() {
	setCurrentPanel(highscoreIndex_);

	auto bar = add<Bar>(gui::BorderLayout::NORTH);
	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    // The component is reused in callbacks.
	highscore_ = add<Highscore>(gui::BorderLayout::CENTER, 10, mw::Color(1, 1, 1), TetrisData::getInstance().getDefaultFont(18));
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initNewHighscorePanel() {
	setCurrentPanel(newHighscoreIndex_);
	add<Bar>(gui::BorderLayout::NORTH);
	
	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	centerPanel->addDefault<Label>("New higscore record!", TetrisData::getInstance().getDefaultFont(40));

	centerPanel->addDefault<TransparentPanel>(400.f, 50.f);
	newHighscorePositionlabel_ = centerPanel->addDefault<Label>("", TetrisData::getInstance().getDefaultFont(32));
	newHighscorePointslabel_ = centerPanel->addDefault<Label>("", TetrisData::getInstance().getDefaultFont(18));
	centerPanel->addDefault<TransparentPanel>(400.f, 50.f);

	auto panel = centerPanel->addDefault<TransparentPanel>(400.f, 50.f);
	panel->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);

	panel->addDefault<Label>("Name: ", TetrisData::getInstance().getDefaultFont(18));	

    textField_ = panel->addDefault<TextField>(TetrisData::getInstance().getDefaultFont(18));
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
			highscore_->addNewRecord(name, getCurrentDate());
			setCurrentPanel(playIndex_);
			saveHighscore();
		}
	});

	centerPanel->addDefault<Button>("Continue", TetrisData::getInstance().getDefaultFont(25))->addActionListener([&](gui::Component& c) {
		textField_->doAction();
	});

	addPanelChangeListener([&](gui::Component& c, bool enterFrame) {
		if (enterFrame) {
			textField_->setFocus(true);
		}
	});
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initCustomPlayPanel() {
	setCurrentPanel(customIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 100.f);
	p1->addDefault<Label>("Width", TetrisData::getInstance().getDefaultFont(18));
	customWidthField_  = p1->addDefault<TextField>("10", TetrisData::getInstance().getDefaultFont(18));
	p1->addDefault<Label>("Height", TetrisData::getInstance().getDefaultFont(18));
	customHeightField_ = p1->addDefault<TextField>("24", TetrisData::getInstance().getDefaultFont(18));

	auto p2 = centerPanel->addDefault<TransparentPanel>(100.f, 100.f);
	p2->addDefault<Label>("Min Level", TetrisData::getInstance().getDefaultFont(18));
	customMinLevel_ = p2->addDefault<TextField>("1", TetrisData::getInstance().getDefaultFont(18));
	p2->addDefault<Label>("Max Level", TetrisData::getInstance().getDefaultFont(18));
	customMaxLevel_ = p2->addDefault<TextField>("24", TetrisData::getInstance().getDefaultFont(18));

	auto button = centerPanel->addDefault<Button>("Play", TetrisData::getInstance().getDefaultFont(30));
	button->addActionListener([&](gui::Component&) {
		int rows = std::stoi(customHeightField_->getText());
		int columns = std::stoi(customWidthField_->getText());
		
		tetrisGame_.closeGame();
		nbrHumans_->setNbr(1);
		nbrAis_->setNbr(0);
		setPlayers(columns, rows);
		tetrisGame_.createLocalGame(columns, rows);

		if (tetrisGame_.getNbrOfPlayers() == 1) {
			tetrisGame_.pause();
		}
		setCurrentPanel(playIndex_);
	});
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initSettingsPanel() {
	setCurrentPanel(settingsIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    auto p = add<TransparentPanel>(gui::BorderLayout::CENTER);
    p->setLayout<gui::VerticalLayout>();
	p->addDefault<Label>("Settings", TetrisData::getInstance().getDefaultFont(30));

	auto checkBox = p->addDefault<CheckBox>("Border around window", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isWindowBordered());
	checkBox->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		bool test = check.isSelected();
		TetrisData::getInstance().setWindowBordered(check.isSelected());
		TetrisData::getInstance().save();
		setBordered(check.isSelected());
	});
	checkBox = p->addDefault<CheckBox>("Fullscreen on double click", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isFullscreenOnDoubleClick());
	checkBox->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setFullscreenOnDoubleClick(check.isSelected());
		TetrisData::getInstance().save();
	});

	checkBox = p->addDefault<CheckBox>("Move the window by holding down left mouse button", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isMoveWindowByHoldingDownMouse());
	checkBox->addActionListener([&](gui::Component& c) {
        auto& check =  static_cast<CheckBox&>(c);
		TetrisData::getInstance().setMoveWindowByHoldingDownMouse(check.isSelected());
		TetrisData::getInstance().save();
	});

	checkBox = p->addDefault<CheckBox>("Vsync", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isWindowVsync());
	checkBox->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setWindowVsync(check.isSelected());
		TetrisData::getInstance().save();
		SDL_GL_SetSwapInterval(check.isSelected() ? 1 : 0);
		updateCurrentFpsLimiter();
	});

	checkBox = p->addDefault<CheckBox>("Fps limiter", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isLimitFps());
	checkBox->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setLimitFps(check.isSelected());
		TetrisData::getInstance().save();
		updateCurrentFpsLimiter();
	});

	checkBox = p->addDefault<CheckBox>("Pause on lost focus", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isWindowPauseOnLostFocus());
	checkBox->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setWindowPauseOnLostFocus(check.isSelected());
		TetrisData::getInstance().save();
	});

	checkBox = p->addDefault<CheckBox>("Show down block", TetrisData::getInstance().getDefaultFont(18));
	checkBox->setSelected(TetrisData::getInstance().isShowDownBlock());
	checkBox->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setShowDownBlock(check.isSelected());
		TetrisData::getInstance().save();
	});
	
	auto label = p->addDefault<Label>("Ai players", TetrisData::getInstance().getDefaultFont(30));
	{
		auto comboBox = p->addDefault<ComboBox>(TetrisData::getInstance().getDefaultFont(20));
		auto ais = TetrisData::getInstance().getAiVector();		
		for (int i = 0; i < (int) ais.size(); ++i) {
			comboBox->addItem(ais[i].getName());
			if (ais[i].getName() == TetrisData::getInstance().getAi1Name()) {
				comboBox->setSelectedItem(i);
			}
			comboBox->addActionListener([&](gui::Component& c) {
				auto& box = static_cast<ComboBox&>(c);
				activeAis_[0] = findAiDevice(box.getChosenItemText());
				TetrisData::getInstance().setAi1Name(box.getChosenItemText());
			});
		}
	}
	{
		auto comboBox = p->addDefault<ComboBox>(TetrisData::getInstance().getDefaultFont(20));
		auto ais = TetrisData::getInstance().getAiVector();
		for (int i = 0; i < (int) ais.size(); ++i) {
			comboBox->addItem(ais[i].getName());
			if (ais[i].getName() == TetrisData::getInstance().getAi1Name()) {
				comboBox->setSelectedItem(i);
			}
			comboBox->addActionListener([&](gui::Component& c) {
				auto& box = static_cast<ComboBox&>(c);
				activeAis_[1] = findAiDevice(box.getChosenItemText());
				TetrisData::getInstance().setAi2Name(box.getChosenItemText());
			});
		}
	}
	{
		auto comboBox = p->addDefault<ComboBox>(TetrisData::getInstance().getDefaultFont(20));
		auto ais = TetrisData::getInstance().getAiVector();
		for (int i = 0; i < (int) ais.size(); ++i) {
			comboBox->addItem(ais[i].getName());
			if (ais[i].getName() == TetrisData::getInstance().getAi1Name()) {
				comboBox->setSelectedItem(i);
			}
			comboBox->addActionListener([&](gui::Component& c) {
				auto& box = static_cast<ComboBox&>(c);
				activeAis_[2] = findAiDevice(box.getChosenItemText());
				TetrisData::getInstance().setAi3Name(box.getChosenItemText());
			});
		}
	}
	{
		auto comboBox = p->addDefault<ComboBox>(TetrisData::getInstance().getDefaultFont(20));
		auto ais = TetrisData::getInstance().getAiVector();
		for (int i = 0; i < (int) ais.size(); ++i) {
			comboBox->addItem(ais[i].getName());
			if (ais[i].getName() == TetrisData::getInstance().getAi1Name()) {
				comboBox->setSelectedItem(i);
			}
			comboBox->addActionListener([&](gui::Component& c) {
				auto& box = static_cast<ComboBox&>(c);
				activeAis_[3] = findAiDevice(box.getChosenItemText());
				TetrisData::getInstance().setAi4Name(box.getChosenItemText());
			});
		}
	}
	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

void TetrisWindow::initNetworkPanel() {
	setCurrentPanel(networkIndex_);
	
	add<Bar>(gui::BorderLayout::NORTH)->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	radioButtonServer_ = centerPanel->addDefault<RadioButton>("   Server", TetrisData::getInstance().getDefaultFont(18));
	radioButtonServer_->setSelected(true);
	radioButtonServer_->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<RadioButton&>(c);
		radioButtonServer_->setSelected(true);
		radioButtonClient_->setSelected(false);
		
		clientIpLabel_->setVisible(false);
		ipClient_->setVisible(false);
	});
	
	radioButtonClient_ = centerPanel->addDefault<RadioButton>("   Client", TetrisData::getInstance().getDefaultFont(18));
	radioButtonClient_->setSelected(false);
	radioButtonClient_->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<RadioButton&>(c);
		radioButtonClient_->setSelected(true);
		radioButtonServer_->setSelected(false);
		
		clientIpLabel_->setVisible(true);
		ipClient_->setVisible(true);
	});

	auto p3 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p3->addDefault<Label>("Port", TetrisData::getInstance().getDefaultFont(18));
	port_ = p3->addDefault<TextField>(std::to_string(TetrisData::getInstance().getPort()), TetrisData::getInstance().getDefaultFont(18));
	
	auto p4 = centerPanel->addDefault<TransparentPanel>(450.f, 60.f);
	p4->setLayout<gui::VerticalLayout>();

	clientIpLabel_ = p3->addDefault<Label>("IP", TetrisData::getInstance().getDefaultFont(18));
	clientIpLabel_->setVisible(false);
	ipClient_ = p3->addDefault<TextField>(TetrisData::getInstance().getIp(), TetrisData::getInstance().getDefaultFont(18));
	ipClient_->setVisible(false);

	errorMessage_ = p4->addDefault<Label>("Connecting ...", TetrisData::getInstance().getDefaultFont(18));
	errorMessage_->setVisible(false);
	
	progressBar_ = p4->addDefault<ProgressBar>();
	progressBar_->setVisible(false);

	networkConnect_ = centerPanel->addDefault<Button>("Connect", TetrisData::getInstance().getDefaultFont(30));

	networkConnect_->addActionListener([&](gui::Component& c) {
		if (tetrisGame_.getStatus() == TetrisGame::Status::WAITING_TO_CONNECT) { // Is connecting!
			std::cout << "WAITING_TO_CONNECT\n";
			errorMessage_->setVisible(true);
			progressBar_->setVisible(true);
			TetrisData::getInstance().setPort(std::stoi(port_->getText()));
			if (radioButtonServer_->isSelected()) {
				TetrisData::getInstance().save();
				tetrisGame_.closeGame();
				nbrHumans_->setNbr(1);
				tetrisGame_.setPlayers(TETRIS_WIDTH, TETRIS_HEIGHT, std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()));
				tetrisGame_.createServerGame(std::stoi(port_->getText()));
			} else {
				TetrisData::getInstance().setIp(ipClient_->getText());
				TetrisData::getInstance().save();
				tetrisGame_.closeGame();
				nbrHumans_->setNbr(1);
				tetrisGame_.setPlayers(TETRIS_WIDTH, TETRIS_HEIGHT, std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()));
				tetrisGame_.createClientGame(std::stoi(port_->getText()), ipClient_->getText());
				addTimerMS(TetrisData::getInstance().getTimeToConnectMS());
				progressBar_->setVisible(true);
				errorMessage_->setText("Connecting ...");
				errorMessage_->setVisible(true);
				networkConnect_->setLabel("Abort");
			}
		} else { // Active connection, abort the current connection!
			tetrisGame_.closeGame();
			networkConnect_->setLabel("Connect");
			progressBar_->setVisible(false);
			errorMessage_->setVisible(false);
			removeLastTimer();
		}
	});

	addPanelChangeListener([&](gui::Component& c, bool enterFrame) {
		if (enterFrame) {
			errorMessage_->setVisible(false);
			networkConnect_->setLabel("Connect");
			removeLastTimer();
		} else { // Leaving frame.
			if (getCurrentPanelIndex() == menuIndex_) {
				// Abort current connection.
				tetrisGame_.closeGame();				
			}
			removeLastTimer();
			errorMessage_->setVisible(true);
			progressBar_->setVisible(false);
			networkConnect_->setLabel("Abort");
		}
	});

	addPanelChangeListener(std::bind(&TetrisWindow::panelChangeListenerFpsLimiter, this, std::placeholders::_1, std::placeholders::_2));
}

// Add a new timer and remove the last.
void TetrisWindow::addTimerMS(unsigned int ms) {
	removeLastTimer();
	lastTimerId_ = SDL_AddTimer(ms, TetrisWindow::addTimer, 0); // Clean up timer?
	if (lastTimerId_ == 0) {
		std::cout << SDL_GetError() << "\n";
	}
}

// Remove last timer. Safe to be called mulltiple times.
void TetrisWindow::removeLastTimer() {
	if (lastTimerId_ != 0) {
		SDL_RemoveTimer(lastTimerId_); // Only call this once per id.
		lastTimerId_ = 0;
	}
}

// Only allowed to be called by addTimer().
Uint32 TetrisWindow::addTimer(Uint32 interval, void* sdlTimerId) {
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0;
	userevent.data1 = nullptr;
	userevent.data2 = nullptr;

	event.type = SDL_USEREVENT;
	event.user = userevent;

	SDL_PushEvent(&event);
	return 0;
}

void TetrisWindow::handleConnectionEvent(TetrisGameEvent& tetrisEvent) {
	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);

		// Test if the player is a local player, exception otherwise.
		auto localPlayer = std::dynamic_pointer_cast<LocalPlayer>(gameOver.player_);
		
		if (tetrisGame_.isDefaultGame() &&
			highscore_->isNewRecord(localPlayer->getPoints())) { // New record only in local game with default settings.
			
			saveCurrentLocalGame();
			
			// Set points in order for highscore to know which point to save in list.
			highscore_->setNextRecord(localPlayer->getPoints(), localPlayer->getClearedRows(), localPlayer->getLevel());
			{
				std::stringstream stream;
				stream << "Position: " << highscore_->getNextPosition();
				newHighscorePositionlabel_->setText(stream.str());
			}
			{
				std::stringstream stream;
				stream << "Points: " << highscore_->getNextRecord();
				newHighscorePointslabel_->setText(stream.str());
			}

			// In order for the user to insert name.
			setCurrentPanel(newHighscoreIndex_);
		}
		updateCurrentFpsLimiter();
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& gamePause = dynamic_cast<GamePause&>(tetrisEvent);
		// Points high enough to be saved in the highscore list?
		if (gamePause.pause_) {
			pauseButton_->setLabel("Unpause");
		} else {
			pauseButton_->setLabel("Pause");
		}
		updateCurrentFpsLimiter();
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& newConnection = dynamic_cast<NewConnection&>(tetrisEvent);
		setCurrentPanel(playIndex_);

		errorMessage_->setVisible(false);
		progressBar_->setVisible(false);
		return;
	} catch (std::bad_cast exp) {}

	try {
		auto& initGame = dynamic_cast<InitGame&>(tetrisEvent);

		// Remove all man buttons for the old remote players.
		for (auto& remoteManButton : remoteManButtons) {
			manBar_->remove(remoteManButton);
		}

		// Add all man buttons for the new remote players.
		for (auto& remoteConnection : initGame.remoteConnections_) {
			// Show remote number of humans.
			auto man = manBar_->addDefault<ManButton>(remoteConnection->getNbrHumanPlayers(), TetrisData::getInstance().getHumanSprite(), TetrisData::getInstance().getCrossSprite());
			man->setNbr(remoteConnection->getNbrHumanPlayers());
			man->setActive(false);
			remoteManButtons.push_back(man);
			// Show remote number of ais.
			man = manBar_->addDefault<ManButton>(remoteConnection->getNbrAiPlayers(), TetrisData::getInstance().getComputerSprite(), TetrisData::getInstance().getCrossSprite());
			man->setNbr(remoteConnection->getNbrAiPlayers());
			man->setActive(false);
			remoteManButtons.push_back(man);
		}
		updateCurrentFpsLimiter();
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& start = dynamic_cast<GameStart&>(tetrisEvent);
		switch (start.status_) {
			case GameStart::LOCAL:
				if (tetrisGame_.isDefaultGame()) {
					menu_->setLabel("Menu");
				} else {
					menu_->setLabel("Abort");
				}
				break;
			case GameStart::CLIENT:
				setTitle("MWetris@Client");
				menu_->setLabel("Abort");
				break;
			case GameStart::SERVER:
				setTitle("MWetris@Server");
				menu_->setLabel("Abort");
				break;
		}
		setCurrentPanel(playIndex_);
		return;
	} catch (std::bad_cast exp) {}
}

void TetrisWindow::loadHighscore() {
	std::vector<HighscoreRecord> highscoreVector = TetrisData::getInstance().getHighscoreRecordVector();
	for (const HighscoreRecord& record : highscoreVector) {
		highscore_->setNextRecord(record.points_, record.rows_, record.level_);
		highscore_->addNewRecord(record.name_, record.date_);
	}
}

void TetrisWindow::saveHighscore() {
	std::vector<HighscoreRecord> highscoreVector;
	for (const auto& score : *highscore_) {
		highscoreVector.emplace_back(score.getName(), score.getDate(),
			score.getPoints(), score.getLevel(), score.getRows());
	}
	TetrisData::getInstance().setHighscoreRecordVector(highscoreVector);
	TetrisData::getInstance().save();
}

void TetrisWindow::setPlayers(int width, int height) {
	std::vector<DevicePtr> playerDevices(devices_.begin(), devices_.begin() + nbrHumans_->getNbr());

	for (unsigned int i = 0; i < nbrAis_->getNbr(); ++i) {
		if (activeAis_[i]) {
			playerDevices.push_back(activeAis_[i]);
		}
	}
	
	tetrisGame_.setPlayers(width, height, playerDevices);
}

DevicePtr TetrisWindow::findHumanDevice(std::string name) const {
	for (const DevicePtr& device: devices_) {
		if (device->getName() == name) {
			return device;
		}
	}
	return devices_[0];
}

DevicePtr TetrisWindow::findAiDevice(std::string name) const {
	auto ais = TetrisData::getInstance().getAiVector();
	for (const Ai& ai : ais) {
		if (ai.getName() == name) {
			return std::make_shared<Computer>(ai);
		}
	}
	return std::make_shared<Computer>(ais.back());
}

void TetrisWindow::sdlEventListener(const SDL_Event& e) {
	switch (e.type) {
		case SDL_USEREVENT: // Abort the current connection.
			progressBar_->setVisible(false);
			errorMessage_->setText("Failed to connect");
			errorMessage_->setVisible(true);
			networkConnect_->setLabel("Connect");
			tetrisGame_.closeGame();
			break;
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
				case SDL_WINDOWEVENT_CLOSE:
					saveCurrentLocalGame();
					break;
				case SDL_WINDOWEVENT_RESIZED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The window is not maximized. Save size!
						TetrisData::getInstance().setWindowWidth(e.window.data1);
						TetrisData::getInstance().setWindowHeight(e.window.data2);
						manBar_->setPreferredSize((float) (e.window.data1 - 100), 100);
					}
					break;
				case SDL_WINDOWEVENT_MOVED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The window is not maximized. Save position!
						TetrisData::getInstance().setWindowPositionX(e.window.data1);
						TetrisData::getInstance().setWindowPositionY(e.window.data2);
					}
					break;
				case SDL_WINDOWEVENT_MAXIMIZED:
					TetrisData::getInstance().setWindowMaximized(true);
					break;
				case SDL_WINDOWEVENT_RESTORED:
					TetrisData::getInstance().setWindowMaximized(false);
					break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
					if (TetrisData::getInstance().isWindowPauseOnLostFocus() &&
						tetrisGame_.getStatus() == TetrisGame::LOCAL &&
						!tetrisGame_.isPaused()) {
						
						tetrisGame_.pause();
					}
					break;
			}
			break;
		case SDL_CONTROLLERDEVICEADDED:
		{
			auto gameControllerPtr = mw::GameController::addController(e.cdevice.which);
			if (gameControllerPtr) {
				std::cout << gameControllerPtr->getName() << std::endl;
				auto gameController = std::make_shared<GameController>(gameControllerPtr);
				devices_.push_back(gameController);
				nbrHumans_->setMax(devices_.size());
			}
		}
			break;
		case SDL_CONTROLLERDEVICEREMOVED:
		{
			int instanceId = e.cdevice.which;
			auto it = std::find_if(devices_.begin(), devices_.end(), [instanceId](const SdlDevicePtr& device) {
				auto gameController = std::dynamic_pointer_cast<GameController>(device);
				return gameController && gameController->getInstanceId() == instanceId;
			});
			if (it != devices_.end()) {
				devices_.erase(it);
				nbrHumans_->setMax(devices_.size());
			}
		}
			break;
		case SDL_MOUSEMOTION:
			if (windowFollowMouse_ && TetrisData::getInstance().isMoveWindowByHoldingDownMouse()) {
#if SDL_VERSION_ATLEAST(2,0,5)
				int mouseX, mouseY;
				SDL_GetGlobalMouseState(&mouseX, &mouseY);
				SDL_SetWindowPosition(TetrisWindow::getSdlWindow(), lastX_ + mouseX - followMouseX_, lastY_ + mouseY - followMouseY_);
#endif
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT) {
				windowFollowMouse_ = true;
#if SDL_VERSION_ATLEAST(2,0,5)
				SDL_GetWindowPosition(mw::Window::getSdlWindow(), &lastX_, &lastY_);
				SDL_GetGlobalMouseState(&followMouseX_, &followMouseY_);
#endif
				if (e.button.clicks == 2 && TetrisData::getInstance().isFullscreenOnDoubleClick()) {
					TetrisWindow::setFullScreen(!TetrisWindow::isFullScreen());
					windowFollowMouse_ = false;
				}
			}
			break;
		case SDL_MOUSEBUTTONUP:
			windowFollowMouse_ = false;
			break;
		case SDL_KEYDOWN:
			if (e.key.keysym.sym == SDLK_ESCAPE) {
				saveCurrentLocalGame();
			}
			break;
	}
}

void TetrisWindow::updateCurrentFpsLimiter() {
	const int DELAY_MS = 10; // Minimum delay supported by most platforms, e.g. Windows.
	if (getCurrentPanelIndex() == playIndex_ && tetrisGame_.isCurrentGameActive() && !tetrisGame_.isPaused()) {
		if (TetrisData::getInstance().isLimitFps()) {
			setLoopSleepingTime(DELAY_MS);
		} else {
			setLoopSleepingTime(-1); // No delay, may cause 100% CPU and GPU usage.
		}
	} else {
		setLoopSleepingTime(DELAY_MS);
	}
}

void TetrisWindow::panelChangeListenerFpsLimiter(gui::Component& c, bool enterFrame) {
	if (enterFrame) {
		updateCurrentFpsLimiter();
	}
}
