#include "tetriswindow.h"
#include "tetrisparameters.h"
#include "tetrisgame.h"
#include "gamecontroller.h"
#include "keyboard.h"
#include "computer.h"
#include "manbutton.h"
#include "highscore.h"
#include "gamecomponent.h"
#include "tetrisentry.h"
#include "guiclasses.h"
#include "tetrisgameevent.h"

#include <gui/borderlayout.h>
#include <gui/flowlayout.h>
#include <gui/verticallayout.h>
#include <gui/gridlayout.h>

#include <mw/sprite.h>
#include <mw/color.h>

#include <ctime>
#include <iostream>
#include <sstream>

TetrisWindow::TetrisWindow(TetrisEntry e, int frame) : tetrisEntry_(e),
	windowFollowMouse_(false), followMouseX_(0), followMouseY_(0),
	nbrOfHumanPlayers_(1), nbrOfComputerPlayers_(0), startFrame_(frame) {

	Frame::setPosition(e.getDeepChildEntry("window positionX").getInt(), e.getDeepChildEntry("window positionY").getInt());
	Frame::setWindowSize(e.getDeepChildEntry("window width").getInt(), e.getDeepChildEntry("window height").getInt());
	Frame::setResizeable(true);
	Frame::setTitle("MWetris");
	Frame::setIcon(e.getDeepChildEntry("window icon").getString());
	Frame::setBordered(e.getDeepChildEntry("window border").getBool());
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
		tetrisEntry_.getDeepChildEntry("window minWidth").getInt(),
		tetrisEntry_.getDeepChildEntry("window minHeight").getInt());

	if (tetrisEntry_.getDeepChildEntry("window maximized").getBool()) {
		SDL_MaximizeWindow(mw::Window::getSdlWindow());
	}

	bool vsync = tetrisEntry_.getDeepChildEntry("window vsync").getBool();
	SDL_GL_SetSwapInterval(vsync ? 1 : 0);

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
	auto background = tetrisEntry_.getDeepChildEntry("window sprites background").getSprite();
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

	auto aiEntry = tetrisEntry_.getDeepChildEntry("activeGames ais player");
	ais_.clear();
	while (aiEntry.hasData()) {
		ais_.push_back(aiEntry.getAi());
		aiEntry = aiEntry.getSibling("player");
	}
}

void TetrisWindow::resumeGame() {
	int rows = tetrisEntry_.getDeepChildEntry("activeGames localGame rows").getInt();
	int columns = tetrisEntry_.getDeepChildEntry("activeGames localGame columns").getInt();

	auto playerEntry = tetrisEntry_.getDeepChildEntry("activeGames localGame player");
	std::vector<PlayerData> playerData;
	while (playerEntry.hasData()) {
		BlockType currentBlockType = playerEntry.getDeepChildEntry("currentBlock blockType").getBlockType();
		int bottomRow = playerEntry.getDeepChildEntry("currentBlock bottomRow").getInt();
		int startColumn = playerEntry.getDeepChildEntry("currentBlock startColumn").getInt();
		int currentRotation = playerEntry.getDeepChildEntry("currentBlock currentRotation").getInt();
		bool ai = playerEntry.getDeepChildEntry("ai").getBool();

		playerData.emplace_back();
		PlayerData& data = playerData.back();
		data.name_ = playerEntry.getDeepChildEntry("name").getString();
		data.points_ = playerEntry.getDeepChildEntry("points").getInt();
		data.level_ = playerEntry.getDeepChildEntry("level").getInt();
		data.levelUpCounter_ = playerEntry.getDeepChildEntry("levelUpCounter").getInt();
		data.current_ = Block(currentBlockType, bottomRow, startColumn, currentRotation);
		data.next_ = playerEntry.getDeepChildEntry("nextBlockType").getBlockType();
		data.board_ = playerEntry.getDeepChildEntry("board").getBlockTypes();
		std::string deviceName = playerEntry.getDeepChildEntry("device name").getString();
		bool deviceAi = playerEntry.getDeepChildEntry("device ai").getBool();
		if (deviceAi) {
			data.device_ = findAiDevice(deviceName);
		} else {
			data.device_ = findHumanDevice(deviceName);
		}
		playerEntry = playerEntry.getSibling("player");
	}
	tetrisGame_.resumeGame(rows, columns, playerData);
}

void TetrisWindow::saveCurrentGame() {
	tetrisEntry_.getDeepChildEntry("activeGames localGame").remove();
	auto localGameTag = tetrisEntry_.getDeepChildEntry("activeGames").addTag("localGame");
	
	localGameTag.addTag("rows", tetrisGame_.getRows());
	localGameTag.addTag("columns", tetrisGame_.getColumns());
	std::vector<PlayerData> dataVector = tetrisGame_.getPlayerData();
	for (PlayerData& data : dataVector) {
		auto playerTag = localGameTag.addTag("player");
		playerTag.addTag("name", data.name_);
		playerTag.addTag("nextBlockType", data.next_);
		playerTag.addTag("levelUpCounter", data.levelUpCounter_);
		playerTag.addTag("ai", false);
		playerTag.addTag("level", data.level_);
		playerTag.addTag("points", data.points_);
		auto blockTag = playerTag.addTag("currentBlock");
		blockTag.addTag("bottomRow", data.current_.getLowestRow());
		blockTag.addTag("blockType", data.current_.getBlockType());
		blockTag.addTag("startColumn", data.current_.getStartColumn());
		blockTag.addTag("currentRotation", data.current_.getCurrentRotation());
		playerTag.addTag("board", data.board_);
		auto deviceTag = playerTag.addTag("device");
		deviceTag.addTag("name", data.device_->getName());
		deviceTag.addTag("ai", data.device_->isAi());
	}
	localGameTag.save();
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

mw::Font TetrisWindow::getDefaultFont(int size) {
	return tetrisEntry_.getDeepChildEntry("window font").getFont(size);
}

void TetrisWindow::updateDevices(gui::Frame& frame, const SDL_Event& windowEvent) {
	for (SdlDevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

TetrisWindow::~TetrisWindow() {
	tetrisEntry_.save();
}

void TetrisWindow::initMenuPanel() {
	setCurrentPanel(menuIndex_);
	
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	resume_ = bar->addDefault<Button>("Resume", getDefaultFont(30), tetrisEntry_);
	resume_->setVisible(true);
	resume_->addActionListener([&](gui::Component&) {
		resumeGame();
		setCurrentPanel(playIndex_);
	});

	// 400 in order to be wide enough for all buttons.
	auto panel = add<TransparentPanel>(gui::BorderLayout::WEST, 400.f);

	panel->setLayout<gui::VerticalLayout>(5.f, 15.f, 10.f);
	panel->setBackgroundColor(1, 1, 1, 0);
	panel->addDefault<Label>("MWetris", getDefaultFont(50), tetrisEntry_);

	panel->addDefaultToGroup<Button>("Play", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		tetrisGame_.closeGame();
		tetrisGame_.setPlayers(std::vector<DevicePtr>(devices_.begin(), devices_.begin() + nbrHumans_->getNbr()));
		tetrisGame_.createLocalGame();

		setCurrentPanel(playIndex_);
		resume_->setVisible(true);
	});

	panel->addDefaultToGroup<Button>("Custom play", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(customIndex_);
	});

	panel->addDefaultToGroup<Button>("Network play", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	panel->addDefaultToGroup<Button>("Highscore", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(highscoreIndex_);
	});

	panel->addDefaultToGroup<Button>("Settings", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(settingsIndex_);
	});

	panel->addDefaultToGroup<Button>("Exit", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		Window::quit();
	});
}

void TetrisWindow::initPlayPanel() {
	setCurrentPanel(playIndex_);

    auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);
	bar->setLayout<gui::BorderLayout>();

	auto p1 = bar->add<TransparentPanel>(gui::BorderLayout::WEST, 400.f, 100.f);
	auto p2 = bar->add<TransparentPanel>(gui::BorderLayout::EAST, 200.f, 100.f);

	p1->setLayout<gui::FlowLayout>(gui::FlowLayout::LEFT, 5.f, 0.f);
	p2->setLayout<gui::FlowLayout>(gui::FlowLayout::RIGHT, 5.f, 0.f);
	
	menu_ = p1->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_);
	menu_->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		if (tetrisGame_.getStatus() == TetrisGame::CLIENT || tetrisGame_.getStatus() == TetrisGame::SERVER) {
			tetrisGame_.closeGame();
			menu_->setLabel("Menu");
			SDL_SetWindowTitle(getSdlWindow(), "MWetris");
		}
	});

	restart_ = p1->addDefault<Button>("Restart", getDefaultFont(30), tetrisEntry_);
	restart_->addActionListener([&](gui::Component&) {
		tetrisGame_.restartGame();
	});	

	nbrHumans_ = p1->addDefault<ManButton>(devices_.size(), tetrisEntry_.getDeepChildEntry("window sprites human").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
	nbrHumans_->addActionListener([&](gui::Component&) {
		setPlayers();
	});

	nbrAis_ = p1->addDefault<ManButton>(4, tetrisEntry_.getDeepChildEntry("window sprites computer").getSprite(), tetrisEntry_.getDeepChildEntry("window sprites cross").getSprite());
	nbrAis_->setNbr(0);
	nbrAis_->addActionListener([&](gui::Component&) {
		setPlayers();
	});

	pauseButton_ = p2->addDefault<Button>("Pause", getDefaultFont(30), tetrisEntry_);
	pauseButton_->addActionListener([&](gui::Component&) {
		tetrisGame_.pause();
	});
	
	addDrawListener([&](gui::Frame& frame, double deltaTime) {
		SDL_GetWindowPosition(getSdlWindow(), &lastX_, &lastY_); // Update last window position.
		tetrisGame_.update(deltaTime);
	});
	
    // Add the game component, already created in the constructor.
	//game_ = add<GameComponent>(gui::BorderLayout::CENTER, tetrisGame_, tetrisEntry_);
	game_ = std::make_shared<GameComponent>(tetrisGame_, tetrisEntry_);
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
	bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    // The component is reused in callbacks.
	highscore_ = add<Highscore>(gui::BorderLayout::CENTER, 10, Color(1, 1, 1), getDefaultFont(18));
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

	panel->addDefault<Button>("Ok", getDefaultFont(18), tetrisEntry_)->addActionListener([&](gui::Component& c) {
		textField_->doAction();
	});
}

void TetrisWindow::initCustomPlayPanel() {
	setCurrentPanel(customIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
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

	bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

    auto p = add<TransparentPanel>(gui::BorderLayout::CENTER);
    p->setLayout<gui::VerticalLayout>();
	p->addDefault<Label>("Settings", getDefaultFont(30), tetrisEntry_);

	auto checkBox1 = p->addDefault<CheckBox>("Border around window", getDefaultFont(18), tetrisEntry_);
	checkBox1->setSelected(tetrisEntry_.getDeepChildEntry("window border").getBool());
	checkBox1->addActionListener([&](gui::Component& c) {
        auto& check = static_cast<CheckBox&>(c);
		bool test = check.isSelected();
		tetrisEntry_.getDeepChildEntry("window border").setBool(check.isSelected());
		tetrisEntry_.save();
		setBordered(check.isSelected());
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

	auto checkBox4 = p->addDefault<CheckBox>("Vsync", getDefaultFont(18), tetrisEntry_);
	checkBox4->setSelected(tetrisEntry_.getDeepChildEntry("window vsync").getBool());
	checkBox4->addActionListener([&](gui::Component& c) {
		auto& check = static_cast<CheckBox&>(c);
		tetrisEntry_.getDeepChildEntry("window vsync").setBool(check.isSelected());
		tetrisEntry_.save();
		SDL_GL_SetSwapInterval(check.isSelected() ? 1 : 0);
	});
}

void TetrisWindow::initCreateServerPanel() {
	setCurrentPanel(createServerIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});

	bar->addDefault<Button>("Client", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(createClientIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p3 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p3->addDefault<Label>("Port", getDefaultFont(18), tetrisEntry_);
	portServer_ = p3->addDefault<TextField>("11155", getDefaultFont(18));

	centerPanel->addDefault<Button>("Connect", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component& c) {
		std::stringstream stream(portServer_->getText());
		int port;
		stream >> port;

		tetrisGame_.closeGame();
		tetrisGame_.createServerGame(port);
	});
}

void TetrisWindow::initCreateClientPanel() {
	setCurrentPanel(createClientIndex_);
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	bar->addDefault<Button>("Menu", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
	});
    
	bar->addDefault<Button>("Server", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(createServerIndex_);
	});

	auto centerPanel = add<TransparentPanel>(gui::BorderLayout::CENTER);
	centerPanel->setLayout<gui::VerticalLayout>();

	auto p1 = centerPanel->addDefault<TransparentPanel>(450.f, 40.f);
	p1->addDefault<Label>("Ip", getDefaultFont(18), tetrisEntry_);
	ipClient_ = p1->addDefault<TextField>("", getDefaultFont(18));
	p1->addDefault<Label>("Port", getDefaultFont(18), tetrisEntry_);
	portClient_ = p1->addDefault<TextField>("11155", getDefaultFont(18));

	centerPanel->addDefault<Button>("Connect", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component& c) {
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
	auto bar = add<Bar>(gui::BorderLayout::NORTH, tetrisEntry_);

	bar->addDefault<Button>("Abort", getDefaultFont(30), tetrisEntry_)->addActionListener([&](gui::Component&) {
		setCurrentPanel(menuIndex_);
		tetrisGame_.closeGame();
	});	

	add<Label>(gui::BorderLayout::CENTER, "Waiting for the server to accept connection!", getDefaultFont(18), tetrisEntry_);
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

void TetrisWindow::setPlayers() {
	std::vector<DevicePtr> playerDevices(devices_.begin(), devices_.begin() + nbrHumans_->getNbr());
	for (unsigned int i = 0; i < nbrAis_->getNbr(); ++i) {
		playerDevices.push_back(std::make_shared<Computer>(activeAis_[i]));
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
	for (const Ai& ai : ais_) {
		if (ai.getName() == name) {
			return std::make_shared<Computer>(ai);
		}
	}
	return std::make_shared<Computer>(activeAis_[0]);
}

void TetrisWindow::sdlEventListener(gui::Frame& frame, const SDL_Event& e) {
	switch (e.type) {
		case SDL_WINDOWEVENT:
			switch (e.window.event) {
				case SDL_WINDOWEVENT_RESIZED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save size!
						tetrisEntry_.getDeepChildEntry("window width").setInt(e.window.data1);
						tetrisEntry_.getDeepChildEntry("window height").setInt(e.window.data2);
					}
					break;
				case SDL_WINDOWEVENT_MOVED:
					if (!(SDL_GetWindowFlags(mw::Window::getSdlWindow()) & SDL_WINDOW_MAXIMIZED)) {
						// The Window's is not maximized. Save position!
						tetrisEntry_.getDeepChildEntry("window positionX").setInt(e.window.data1);
						tetrisEntry_.getDeepChildEntry("window positionY").setInt(e.window.data2);
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
		case SDL_CONTROLLERDEVICEADDED:
		{
			auto gameControllerPtr = mw::GameController::addController(e.cdevice.which);
			if (gameControllerPtr) {
				std::cout << gameControllerPtr->getName() << std::endl;
				devices_.push_back(std::make_shared<GameController>(gameControllerPtr));
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
			if (windowFollowMouse_ && tetrisEntry_.getDeepChildEntry("window moveWindowByHoldingDownMouse").getBool()) {
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
				if (e.button.clicks == 2 && tetrisEntry_.getDeepChildEntry("window fullscreenOnDoubleClick").getBool()) {
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
