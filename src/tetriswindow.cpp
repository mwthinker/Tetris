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

TetrisWindow::TetrisWindow(int frame) : 
	windowFollowMouse_(false), followMouseX_(0), followMouseY_(0),
	nbrOfHumanPlayers_(1), nbrOfComputerPlayers_(0), startFrame_(frame) {

	Frame::setPosition(TetrisData::getInstance().getWindowPositionX(), TetrisData::getInstance().getWindowPositionY());
	Frame::setWindowSize(TetrisData::getInstance().getWindowWidth(), TetrisData::getInstance().getWindowHeight());
	Frame::setResizeable(TetrisData::getInstance().getWindowWidth());
	Frame::setTitle("MWetris");
	Frame::setIcon(TetrisData::getInstance().getWindowIcon());
	Frame::setBordered(TetrisData::getInstance().isWindowBordered());
	Frame::setDefaultClosing(true);
}

void TetrisWindow::initOpenGl() {
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
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

	addSdlEventListener(std::bind(&TetrisWindow::updateDevices, this, std::placeholders::_1, std::placeholders::_2));

	// Saves the last window position. And makes the window movable by holding down left mouse button.
	addSdlEventListener(std::bind(&TetrisWindow::sdlEventListener, this, std::placeholders::_1, std::placeholders::_2));

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
	createClientIndex_ = pushBackPanel(std::make_shared<Background>(background));
	createServerIndex_ = pushBackPanel(std::make_shared<Background>(background));
	waitToConnectIndex_ = pushBackPanel(std::make_shared<Background>(background));

	initMenuPanel();
	initPlayPanel();
	initHighscorePanel();
	initNewHighscorePanel();
	initCustomPlayPanel();
	initSettingsPanel();
	initCreateServerPanel();
	initCreateClientPanel();
	initWaitToConnectPanel();

	if (startFrame_ >= 0 && startFrame_ <= waitToConnectIndex_) {
		if (startFrame_ == playIndex_) {
			// Initialization local game settings.
			tetrisGame_.createLocalGame();
		}
		setCurrentPanel(startFrame_);
	}

	loadHighscore();
	// Init ai players.
	activeAis_[0] = findAiDevice(TetrisData::getInstance().getAi1Name());
	activeAis_[1] = findAiDevice(TetrisData::getInstance().getAi2Name());
	activeAis_[2] = findAiDevice(TetrisData::getInstance().getAi3Name());
	activeAis_[3] = findAiDevice(TetrisData::getInstance().getAi4Name());
}

void TetrisWindow::resumeGame() {
	int rows = TetrisData::getInstance().getActiveLocalGameRows();
	int columns = TetrisData::getInstance().getActiveLocalGameColumns();

	std::vector<PlayerData> playerDataVector = TetrisData::getInstance().getActiveLocalGamePlayers();
	for (PlayerData& playerData : playerDataVector) {
		if (playerData.ai_) {
			playerData.device_ = findAiDevice(playerData.deviceName_);
		} else {
			playerData.device_ = findHumanDevice(playerData.deviceName_);
		}
	}
	tetrisGame_.resumeGame(rows, columns, playerDataVector);
}

void TetrisWindow::saveCurrentGame() {
	TetrisData::getInstance().setActiveLocalGame(tetrisGame_.getRows(), tetrisGame_.getColumns(), tetrisGame_.getPlayerData());
	TetrisData::getInstance().save();
}

void TetrisWindow::startServer(int port) {
	std::stringstream stream;
	stream << port;
	portServer_->setText(stream.str());

	tetrisGame_.closeGame();
	tetrisGame_.createServerGame(port);
}

void TetrisWindow::startClient(int port, std::string ip) {
	std::stringstream stream;
	stream << port;
	portClient_->setText(stream.str());
	ipClient_->setText(ip);

	tetrisGame_.closeGame();
	tetrisGame_.createClientGame(port, ipClient_->getText());

	setCurrentPanel(waitToConnectIndex_);
}

void TetrisWindow::updateDevices(gui::Frame& frame, const SDL_Event& windowEvent) {
	for (SdlDevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

TetrisWindow::~TetrisWindow() {
	TetrisData::getInstance().save();
}

void TetrisWindow::initMenuPanel() {
	setCurrentPanel(menuIndex_);
	
	auto bar = add<Bar>(gui::BorderLayout::NORTH);
	resume_ = bar->addDefault<Button>("Resume", TetrisData::getInstance().getDefaultFont(30));
	resume_->setVisible(true);
	resume_->addActionListener([&](gui::Component&) {
		resumeGame();
		setCurrentPanel(playIndex_);
	});

	// 400 in order to be wide enough for all buttons.
	auto panel = add<TransparentPanel>(gui::BorderLayout::WEST, 400.f);

	panel->setLayout<gui::VerticalLayout>(5.f, 15.f, 10.f);
	panel->setBackgroundColor(1, 1, 1, 0);
	panel->addDefault<Label>("MWetris", TetrisData::getInstance().getDefaultFont(50));

	panel->addDefaultToGroup<Button>("Play", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.setPlayers(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()));
		tetrisGame_.createLocalGame();

		setCurrentPanel(playIndex_);
		resume_->setVisible(true);

	});

	panel->addDefaultToGroup<Button>("Custom play", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(customIndex_);
	});

	panel->addDefaultToGroup<Button>("Network play", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	panel->addDefaultToGroup<Button>("Highscore", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(highscoreIndex_);
	});

	panel->addDefaultToGroup<Button>("Settings", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(settingsIndex_);
	});

	panel->addDefaultToGroup<Button>("Exit", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		Window::quit();
	});
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

    auto bar = add<Bar>(gui::BorderLayout::NORTH);
	bar->setLayout<gui::BorderLayout>();

	auto p1 = bar->add<TransparentPanel>(gui::BorderLayout::WEST, 400.f, 100.f);
	auto p2 = bar->add<TransparentPanel>(gui::BorderLayout::EAST, 200.f, 100.f);

	p1->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
	p2->setLayout<gui::FlowLayout>(gui::FlowLayout::RIGHT, 5.f, 0.f);
	
	menu_ = p1->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30));
	menu_->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		if (tetrisGame_.getStatus() == TetrisGame::CLIENT || tetrisGame_.getStatus() == TetrisGame::SERVER) {
			tetrisGame_.closeGame();
			menu_->setLabel("Menu");
			SDL_SetWindowTitle(getSdlWindow(), "MWetris");
		}
	});

	restart_ = p1->addDefault<Button>("Restart", TetrisData::getInstance().getDefaultFont(30));
	restart_->addActionListener([&](gui::Component&) {
		tetrisGame_.restartGame();
	});	

	nbrHumans_ = p1->addDefault<ManButton>(devices_.size(), TetrisData::getInstance().getHumanSprite(), TetrisData::getInstance().getCrossSprite());
	nbrHumans_->addActionListener([&](gui::Component&) {
		setPlayers();
	});

	nbrAis_ = p1->addDefault<ManButton>(activeAis_.size(), TetrisData::getInstance().getComputerSprite(), TetrisData::getInstance().getCrossSprite());
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component&) {
		setPlayers();
	});

	pauseButton_ = p2->addDefault<Button>("Pause", TetrisData::getInstance().getDefaultFont(30));
	pauseButton_->addActionListener([&](gui::Component&) {
		tetrisGame_.pause();
	});
	
	addDrawListener([&](gui::Frame& frame, double deltaTime) {
		SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_); // Update last window position.
		tetrisGame_.update(deltaTime);
	});
	
    // Add the game component, already created in the constructor.
	//game_ = add<GameComponent>(gui::BorderLayout::CENTER, tetrisGame_, tetrisEntry_);
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
}

void TetrisWindow::initHighscorePanel() {
	setCurrentPanel(highscoreIndex_);

	auto bar = add<Bar>(gui::BorderLayout::NORTH);
	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    // The component is reused in callbacks.
	highscore_ = add<Highscore>(gui::BorderLayout::CENTER, 10, Color(1, 1, 1), TetrisData::getInstance().getDefaultFont(18));
}

void TetrisWindow::initNewHighscorePanel() {
	setCurrentPanel(newHighscoreIndex_);
	add<Bar>(gui::BorderLayout::NORTH);

	auto panel = add<TransparentPanel>(gui::BorderLayout::CENTER);
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
			std::time_t t = std::time(NULL);
			char mbstr[30];
			std::strftime(mbstr, 30, "%Y-%m-%d", std::localtime(&t));
			highscore_->addNewRecord(name, mbstr);
			setCurrentPanel(menuIndex_);
			saveHighscore();
		}
	});

	panel->addDefault<Button>("Ok", TetrisData::getInstance().getDefaultFont(18))->addActionListener([&](gui::Component& c) {
		textField_->doAction();
	});
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

	centerPanel->addDefault<Button>("Play", TetrisData::getInstance().getDefaultFont(30));
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

	auto checkBox1 = p->addDefault<CheckBox>("Border around window", TetrisData::getInstance().getDefaultFont(18));
	checkBox1->setSelected(TetrisData::getInstance().isWindowBordered());
	checkBox1->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		bool test = check.isSelected();
		TetrisData::getInstance().setWindowBordered(check.isSelected());
		TetrisData::getInstance().save();
		setBordered(check.isSelected());
	});
	auto checkBox2 = p->addDefault<CheckBox>("Fullscreen on double click", TetrisData::getInstance().getDefaultFont(18));
	checkBox2->setSelected(TetrisData::getInstance().isFullscreenOnDoubleClick());
	checkBox2->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setFullscreenOnDoubleClick(check.isSelected());
		TetrisData::getInstance().save();
	});

	auto checkBox3 = p->addDefault<CheckBox>("Move the window by holding down left mouse button", TetrisData::getInstance().getDefaultFont(18));
	checkBox3->setSelected(TetrisData::getInstance().isMoveWindowByHoldingDownMouse());
	checkBox3->addActionListener([&](gui::Component& c) {
        auto& check =  static_cast<CheckBox&>(c);
		TetrisData::getInstance().setMoveWindowByHoldingDownMouse(check.isSelected());
		TetrisData::getInstance().save();
	});

	auto checkBox4 = p->addDefault<CheckBox>("Vsync", TetrisData::getInstance().getDefaultFont(18));
	checkBox4->setSelected(TetrisData::getInstance().isWindowVsync());
	

	checkBox4->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		TetrisData::getInstance().setWindowVsync(check.isSelected());
		TetrisData::getInstance().save();
		SDL_GL_SetSwapInterval(check.isSelected() ? 1 : 0);
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
}

void TetrisWindow::initCreateServerPanel() {
	setCurrentPanel(createServerIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	bar->addDefault<Button>("Client", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(createClientIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p3 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p3->addDefault<Label>("Port", TetrisData::getInstance().getDefaultFont(18));
	portServer_ = p3->addDefault<TextField>("11155", TetrisData::getInstance().getDefaultFont(18));

	centerPanel->addDefault<Button>("Connect", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component& c) {
		std::stringstream stream(portServer_->getText());
		int port;
		stream >> port;

		tetrisGame_.closeGame();
		tetrisGame_.createServerGame(port);
	});
}

void TetrisWindow::initCreateClientPanel() {
	setCurrentPanel(createClientIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	bar->addDefault<Button>("Menu", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
    
	bar->addDefault<Button>("Server", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p1->addDefault<Label>("Ip", TetrisData::getInstance().getDefaultFont(18));
	ipClient_ = p1->addDefault<TextField>("", TetrisData::getInstance().getDefaultFont(18));
	p1->addDefault<Label>("Port", TetrisData::getInstance().getDefaultFont(18));
	portClient_ = p1->addDefault<TextField>("11155", TetrisData::getInstance().getDefaultFont(18));

	centerPanel->addDefault<Button>("Connect", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component& c) {
		int port;
		std::stringstream stream1;
		stream1 << portClient_->getText();
		stream1 >> port;

		tetrisGame_.closeGame();
		tetrisGame_.createClientGame(port, ipClient_->getText());
		setCurrentPanel(waitToConnectIndex_);
	});
}

void TetrisWindow::initWaitToConnectPanel() {
	setCurrentPanel(waitToConnectIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH);

	bar->addDefault<Button>("Abort", TetrisData::getInstance().getDefaultFont(30))->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});	

	add<Label>(gui::BorderLayout::CENTER, "Waiting for the server to accept connection!", TetrisData::getInstance().getDefaultFont(18));
}

void TetrisWindow::handleConnectionEvent(TetrisGameEvent& tetrisEvent) {
	try {
		auto& gameOver = dynamic_cast<GameOver&>(tetrisEvent);
		// Points high enough to be saved in the highscore list?
		if (highscore_->isNewRecord(gameOver.points_)) {
			// Set points in order for highscore to know which point to save in list.
			highscore_->setNextRecord(gameOver.points_);
			// In order for the user to insert name.
			setCurrentPanel(newHighscoreIndex_);
		}
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
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& newConnection = dynamic_cast<NewConnection&>(tetrisEvent);
		setCurrentPanel(playIndex_);
		return;
	} catch (std::bad_cast exp) {}
	
	try {
		auto& start = dynamic_cast<GameStart&>(tetrisEvent);
		switch (start.status_) {
			case GameStart::LOCAL:
				menu_->setLabel("Menu");
				break;
			case GameStart::CLIENT:
				SDL_SetWindowTitle(getSdlWindow(), "MWetris@Client");
				menu_->setLabel("Abort");
				break;
			case GameStart::SERVER:
				SDL_SetWindowTitle(getSdlWindow(), "MWetris@Server");
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
		highscore_->setNextRecord(record.points_);
		highscore_->addNewRecord(record.name_, record.date_);
	}
}

void TetrisWindow::saveHighscore() {
	std::vector<HighscoreRecord> highscoreVector;
	for (const auto& score : *highscore_) {
		highscoreVector.emplace_back(score.name_.getText(), score.date_.getText(), score.intPoints_);
	}
	TetrisData::getInstance().setHighscoreRecordVector(highscoreVector);
	TetrisData::getInstance().save();
}

void TetrisWindow::setPlayers() {
	std::vector<DevicePtr> playerDevices(devices_.begin(), devices_.begin() + nbrHumans_->getNbr());

	for (unsigned int i = 0; i < nbrAis_->getNbr(); ++i) {
		if (activeAis_[i]) {
			playerDevices.push_back(activeAis_[i]);
		}
	}
	
	tetrisGame_.setPlayers(playerDevices);
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
	ais.push_back(Ai()); // Add default ai.

	for (const Ai& ai : ais) {
		if (ai.getName() == name) {
			return std::make_shared<Computer>(ai);
		}
	}
	return std::make_shared<Computer>(ais.back());
}

void TetrisWindow::sdlEventListener(gui::Frame& frame, const SDL_Event& e) {
	switch (e.type) {
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save size!
						TetrisData::getInstance().setWindowWidth(e.window.data1);
						TetrisData::getInstance().setWindowHeight(e.window.data2);
					}
					break;
				case SDL_WINDOWEVENT_MOVED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save position!
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
				saveCurrentGame();
			}
			break;
		default:
			break;
	}
}
