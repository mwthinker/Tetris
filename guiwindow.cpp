#include "guiwindow.h"
#include "barcolor.h"
#include "imagebackground.h"
#include "textbutton.h"
#include "group.h"
#include "multiframe.h"
#include "textitem.h"
#include "textboxdraw.h"
#include "inputnumberformatter.h"
#include "manbutton.h"
#include "highscore.h"
#include "networklooby.h"
#include "device.h"

#include "gamesprite.h"
#include "gamefont.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>

GuiWindow::GuiWindow() : mw::Window(520,640,"MWetris","images/tetris.bmp") {
	textColor_ = mw::Color(1,1,1);
	textButtonColor_ = mw::Color(1,.1,.1);
	focusColor_ = mw::Color(.8, .1, 0, .3);
	onHoverColor_ = mw::Color(.6, .1, .1);
	notHoverColor_ = mw::Color(.4, .0, .0, .0);
	pushedColor_ = mw::Color(.8, .0, 0, .7);
	barColor_ = mw::Color(.5,0,0,.30);
	hDistance_ = 30;

	auto background = gui::createImageBackground(spriteBackground);
	multiFrame_.setDefaultBackground(background);

	playFrameIndex_ = multiFrame_.addFrameBack();
	highscoreFrameIndex_ = multiFrame_.addFrameBack();
	customFrameIndex_ = multiFrame_.addFrameBack();
	optionFrameIndex_ = multiFrame_.addFrameBack();
	newHighscoreFrameIndex_ = multiFrame_.addFrameBack();
	networkFrameIndex_ = multiFrame_.addFrameBack();
	createClientFrameIndex_ = multiFrame_.addFrameBack();
	createServerFrameIndex_ = multiFrame_.addFrameBack();
	loobyClientFrameIndex_ = multiFrame_.addFrameBack();
	loobyServerFrameIndex_ = multiFrame_.addFrameBack();
	waitToConnectFrameIndex_ = multiFrame_.addFrameBack();
	networkPlayFrameIndex_ = multiFrame_.addFrameBack();
	aiFrameIndex_ = multiFrame_.addFrameBack();

	initFrameMenu();
	initHighscoreFrame();
	initPlayFrame();
	initCustomPlayFrame();
	initNewHighScoreFrame();
	initCreateServerFrame();
	initCreateClientFrame();
	initServerLoobyFrame();
	initClientLoobyFrame();
	initWaitToConnectFrame();
	initNetworkPlayFrame();
	initAiFrame();

	multiFrame_.setCurrentFrame(0);
	mw::Window::setUnicodeInputEnable(true);
	resize(getWidth(),getHeight());

	pauseKey_ = SDLK_p;
	restartKey_ = SDLK_F2;
}

HighscorePtr GuiWindow::getHighscorePtr() const {
    return highscorePtr_;
}

gui::TextButtonPtr GuiWindow::getPausePtr() const {
    return pause_;
}

NetworkLoobyPtr GuiWindow::getNetworkLoobyPtr() const {
    return networkLoobyPtr_;
}

gui::TextButtonPtr GuiWindow::getReadyPtr() const {
	return ready_;
}

void GuiWindow::gotoLocalPlayFrame() {
	multiFrame_.setCurrentFrame(playFrameIndex_);
}

void GuiWindow::gotoServerPlayFrame() {
	multiFrame_.setCurrentFrame(networkPlayFrameIndex_);
}

void GuiWindow::gotoClientPlayFrame() {
	multiFrame_.setCurrentFrame(networkPlayFrameIndex_);
}

void GuiWindow::gotoServerLoobyFrame() {
	multiFrame_.setCurrentFrame(loobyServerFrameIndex_);
}

void GuiWindow::gotoClientLoobyFrame() {
	multiFrame_.setCurrentFrame(loobyClientFrameIndex_);
}

bool GuiWindow::isUpdatingGame() const {
	return multiFrame_.getCurrentFrameIndex() == playFrameIndex_ || multiFrame_.getCurrentFrameIndex() == networkPlayFrameIndex_;
}

gui::TextButtonPtr GuiWindow::createButton(std::string text, int size, std::function<void(gui::GuiItem*)> onClick) {
	gui::TextButtonPtr button = gui::createTextButton(text, size, fontDefault,
		textButtonColor_, focusColor_, onHoverColor_, notHoverColor_, pushedColor_);
	button->addOnClickListener(onClick);
	return button;
}

gui::TextBoxPtr GuiWindow::createTextBox(int size) {
	return gui::createTextBoxDraw(size, fontDefault, 30, textColor_, focusColor_, onHoverColor_);
}

gui::BarColorPtr GuiWindow::createUpperBar() {
	return gui::createBarColor(gui::Bar::UP, hDistance_, barColor_);
}

void GuiWindow::initFrameMenu() {
	multiFrame_.setCurrentFrame(0);
	multiFrame_.add(gui::createTextItem("MWetris", fontDefault50, 80, textColor_),10,50,false,true);
	multiFrame_.add(gui::createTextItem("Made by Marcus Welander", fontDefault18, 12, textColor_),0,0,true,false);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	// Text is outside due to invY which dont know where the top of the text begin.
	// The bottom is put where it is supposed to but the invers not.
	gui::ButtonPtr b1 = createButton("Resume", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(playFrameIndex_);
	});
	resumeButton_ = b1;
	resumeButton_->setVisible(false);

	// Menu.
	gui::ButtonPtr b2 = createButton("Play", 35, [&](gui::GuiItem*) {
		resumeButton_->setVisible(true);
		abortGame();
		createLocalGame(TETRIS_WIDTH, TETRIS_HEIGHT, TETRIS_MAX_LEVEL);
	});
	b2->setFocus(true);

	gui::ButtonPtr b3 = createButton("Custom play", 35, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(customFrameIndex_);
	});
	gui::ButtonPtr b4 = createButton("Highscore", 35, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(highscoreFrameIndex_);
	});
	gui::ButtonPtr b5 = createButton("Options", 35, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(optionFrameIndex_);
	});
	gui::ButtonPtr b6 = createButton("Exit", 35, [&](gui::GuiItem*) {
		quit();
	});
	b6->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			quit();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				quit();
				break;
			}
		}
	});

	gui::ButtonPtr b7 = createButton("Network game", 35, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(createServerFrameIndex_);
	});

	int x = 10;
	int y = 150;
	int distance = 50;

	// In Bar.
	multiFrame_.add(b1,0,0,false,true);

	// Menu.
	multiFrame_.add(b2,x,y,false,true);
	y += distance;
	multiFrame_.add(b3,x,y,false,true);
	y += distance;
	multiFrame_.add(b7,x,y,false,true);
	y += distance;
	multiFrame_.add(b4,x,y,false,true);
	y += distance;
	multiFrame_.add(b5,x,y,false,true);
	y += distance;
	multiFrame_.add(b6,x,y,false,true);

	// Focus is switchable by the up and down arrow.
	gui::GroupPtr group = gui::createGroup(SDLK_UP,SDLK_DOWN);
	group->add(b2);
	group->add(b3);
	group->add(b7);
	group->add(b4);
	group->add(b5);
	group->add(b6);
	group->add(b1);

	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initPlayFrame() {
	multiFrame_.setCurrentFrame(playFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::GuiItem* item) {
		multiFrame_.setCurrentFrame(0);
		item->setFocus(false);
		if (!isPaused()) {
			changePauseState();
		}
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	gui::ButtonPtr b2 = createButton("Restart", hDistance_, [&](gui::GuiItem* item) {
		restartGame();
		item->setFocus(false);
	});
	b2->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == restartKey_) {
				item->click();
				break;
			}
		}
	});

	auto manButton = createManButton(hDistance_, spriteMan, spriteCross);
	manButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfHumanPlayers(nbr+1);
		int newNbr = getNbrOfHumanPlayers();

		if (newNbr == nbr) {
            newNbr = 1;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfHumanPlayers(newNbr);

		createLocalGame();
	});

	auto humanButton = createManButton(hDistance_, spriteMan, spriteCross);
	humanButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfHumanPlayers(nbr+1);
		int newNbr = getNbrOfHumanPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfHumanPlayers(newNbr);

		createLocalGame();
	});

	humanButton->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		switch (sdlEvent.type) {
		case SDL_MOUSEBUTTONUP:
			switch (sdlEvent.button.button) {
			case SDL_BUTTON_RIGHT:
				if (nbrOfPlayers->isMouseInside()) {
					int nbr = nbrOfPlayers->getNbr();
					if (nbr == 0) {
						int newNbr = getNbrOfHumanPlayers();
						setNbrOfHumanPlayers(newNbr);
						while (newNbr == nbr) {
							++newNbr;
							setNbrOfHumanPlayers(newNbr);
							nbrOfPlayers->setNbr(newNbr);
							newNbr = getNbrOfHumanPlayers();
							nbr = nbrOfPlayers->getNbr();
						}
					}

					setNbrOfHumanPlayers(nbr-1);
					nbrOfPlayers->setNbr(nbr-1);
					createLocalGame();
				}
			}
		}
	});

	auto computerButton = createManButton(hDistance_, spriteComputer, spriteCross);
	computerButton->setNbr(0);
	computerButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfComputerPlayers(nbr+1);
		int newNbr = getNbrOfComputerPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfComputerPlayers(newNbr);

		createLocalGame();
	});
	computerButton->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		switch (sdlEvent.type) {
		case SDL_MOUSEBUTTONUP:
			switch (sdlEvent.button.button) {
			case SDL_BUTTON_RIGHT:
				if (nbrOfPlayers->isMouseInside()) {
					int nbr = nbrOfPlayers->getNbr();
					if (nbr == 0) {
						int newNbr = getNbrOfComputerPlayers();
						setNbrOfComputerPlayers(newNbr);
						while (newNbr == nbr) {
							++newNbr;
							setNbrOfComputerPlayers(newNbr);
							nbrOfPlayers->setNbr(newNbr);
							newNbr = getNbrOfComputerPlayers();
							nbr = nbrOfPlayers->getNbr();
						}
					}

					setNbrOfComputerPlayers(nbr-1);
					nbrOfPlayers->setNbr(nbr-1);
					createLocalGame();
				}
			}
		}
	});

	pause_ = createButton("Pause", hDistance_, [&](gui::GuiItem* item) {
		changePauseState();
		item->setFocus(false);
	});
	pause_->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == pauseKey_ || key == SDLK_PAUSE) {
				item->click();
				break;
			}
		}
	});

	multiFrame_.add(b1,0,0,false,true);
	multiFrame_.add(b2,80,0,false,true);
	multiFrame_.add(humanButton, 80 + b2->getWidth(), 0, false, true);
	multiFrame_.add(computerButton, 100 + b2->getWidth() + 130, 0, false, true);
	multiFrame_.add(pause_,0,0,true,true);
}

void GuiWindow::initHighscoreFrame() {
	multiFrame_.setCurrentFrame(highscoreFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE || key == SDLK_RETURN || key == SDLK_KP_ENTER) {
				item->click();
				break;
			}
		}
	});

	highscorePtr_ = createHighscore(10, textColor_);
	highscorePtr_->addOnClickListener([&](gui::GuiItem* item) {
        multiFrame_.setCurrentFrame(newHighscoreFrameIndex_);
	});

	multiFrame_.add(highscorePtr_, 10, (int) (hDistance_*1.2), false, true);

	multiFrame_.add(b1,0,0,false,true);
}

void GuiWindow::initCustomPlayFrame() {
	multiFrame_.setCurrentFrame(customFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr menu = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	menu->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	multiFrame_.add(menu, 0, 0, false, true);

	std::stringstream stream;
	// Set board size ------------------------------------------------------
	gui::TextItemPtr textItem = gui::createTextItem("Width", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem,45,50,false,true);
	customPlayWidth_ = createTextBox(35);
	customPlayWidth_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	stream << TETRIS_WIDTH;
	customPlayWidth_->setText(stream.str());
	multiFrame_.add(customPlayWidth_,100,50,false,true);

	gui::TextItemPtr textItem2 = gui::createTextItem("Height", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem2,140,50,false,true);
	customPlayHeight_ = createTextBox(35);
	customPlayHeight_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	stream.str("");
	stream << TETRIS_HEIGHT;
	customPlayHeight_->setText(stream.str());
	multiFrame_.add(customPlayHeight_,200,50,false,true);

	// Set max level -----------------------------------------------------
	gui::TextItemPtr textItem3 = gui::createTextItem("Max Level", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem3,45,100,false,true);
	customPlaymaxLevel_ = createTextBox(35);
	customPlaymaxLevel_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	stream.str("");
	stream << TETRIS_MAX_LEVEL;
	customPlaymaxLevel_->setText(stream.str());
	multiFrame_.add(customPlaymaxLevel_,140,100,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Create game", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(playFrameIndex_);
		std::stringstream stream;
		stream << customPlayWidth_->getText() << " ";
		stream << customPlayHeight_->getText() << " ";
		stream << customPlaymaxLevel_->getText() << " ";
		int width, height, maxLevel;
		stream >> width >> height >> maxLevel;
		createLocalGame(width,height,maxLevel);
		resumeButton_->setVisible(true);
	});

	multiFrame_.add(button,45,150,false,true);

	// Add all items to group!
	gui::GroupPtr group = gui::createGroup(SDLK_UP,SDLK_DOWN);
	group->add(customPlayWidth_);
	group->add(customPlayHeight_);
	group->add(customPlaymaxLevel_);
	group->add(button);
	group->add(menu);

	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initOptionFrame(const std::vector<DevicePtr>& devices) {
	// Upper bar.
	multiFrame_.addBar(createUpperBar(), optionFrameIndex_);

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	gui::ButtonPtr b2 = createButton("AI", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(aiFrameIndex_);
	});

	gui::TextItemPtr textItem1 = gui::createTextItem("Connected players", fontDefault, 30, textColor_);
	multiFrame_.add(textItem1, 0, hDistance_, false, true, optionFrameIndex_);
	int size = devices.size();
	for (int i = 0; i < size; ++i) {
		std::stringstream stream;
		stream << "Player " << 1+i << ": " << devices[i]->getName();
		gui::TextItemPtr textItem2 = gui::createTextItem(stream.str(), fontDefault18, 18, textColor_);
		multiFrame_.add(textItem2, 0, 70 + i * 30, false, true, optionFrameIndex_);
	}

	multiFrame_.add(b1, 0, 0, false, true, optionFrameIndex_);
	multiFrame_.add(b2, 80, 0, false, true, optionFrameIndex_);
}

void GuiWindow::initServerLoobyFrame() {
	multiFrame_.setCurrentFrame(loobyServerFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	ready_ = createButton("Ready", hDistance_, [&] (gui::GuiItem* item) {
		changeReadyState();
	});

	gui::ButtonPtr b2 = createButton("Start", hDistance_, [&](gui::GuiItem* item) {
		startGame();
	});

	networkLoobyPtr_ = createNetworkLooby();

	multiFrame_.add(b1,0,0,false,true);
	multiFrame_.add(networkLoobyPtr_, 0, 100, false,true);
	multiFrame_.add(ready_,0,400,false,true);
	multiFrame_.add(b2,100,400,false,true);
}

void GuiWindow::initClientLoobyFrame() {
	multiFrame_.setCurrentFrame(loobyClientFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	multiFrame_.add(b1,0,0,false,true);
	multiFrame_.add(networkLoobyPtr_, 0, 100, false,true);
	multiFrame_.add(ready_,0,400,false,true);
}

void GuiWindow::initCreateServerFrame() {
	multiFrame_.setCurrentFrame(createServerFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr header = gui::createTextItem("Server", fontDefault, 30, textColor_);

	gui::ButtonPtr menu = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	menu->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});
	gui::ButtonPtr b1 = createButton("Client", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(createClientFrameIndex_);
	});

	multiFrame_.add(menu,0,0,false,true);
	multiFrame_.add(header,0,50,false,true);
	multiFrame_.add(b1,80,0,false,true);

	auto manButton = createManButton(hDistance_, spriteMan, spriteCross);
	manButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfHumanPlayers(nbr+1);
		int newNbr = getNbrOfHumanPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfHumanPlayers(newNbr);
	});

	auto computerButton = createManButton(hDistance_, spriteComputer, spriteCross);
	computerButton->setNbr(0);
	computerButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfComputerPlayers(nbr+1);
		int newNbr = getNbrOfComputerPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfComputerPlayers(newNbr);
	});

	// Set board size ------------------------------------------------------
	gui::TextItemPtr textItem = gui::createTextItem("Width", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem,0,100,false,true);
	multiFrame_.add(customPlayWidth_,55,100,false,true);

	gui::TextItemPtr textItem2 = gui::createTextItem("Height", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem2,95,100,false,true);
	multiFrame_.add(customPlayHeight_,155,100,false,true);

	gui::TextItemPtr textItem3 = gui::createTextItem("Port", fontDefault18, 18, textColor_);
	portBox_ = createTextBox(105);
	portBox_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(6));
	portBox_->setText("11155");
	multiFrame_.add(textItem3,0,150,false,true);
	multiFrame_.add(portBox_,55,150,false,true);

	gui::TextItemPtr textItem4 = gui::createTextItem("Local players:", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem4,0,200,false,true);
	multiFrame_.add(manButton,120,200,false,true);
	multiFrame_.add(computerButton,260,200,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Create", 30, [&](gui::GuiItem*) {
		std::stringstream stream;
		stream << customPlayWidth_->getText() << " ";
		stream << customPlayHeight_->getText() << " ";
		stream << portBox_->getText();
		int width, height, port;
		stream >> width >> height >> port;
		createServerGame(port, width, height);
		resumeButton_->setVisible(false);
		multiFrame_.setCurrentFrame(loobyServerFrameIndex_);
	});

	multiFrame_.add(button,0,250,false,true);

	// Add all items to group!
	gui::GroupPtr group = gui::createGroup(SDLK_UP,SDLK_DOWN);
	group->add(menu);
	group->add(b1);
	group->add(customPlayWidth_);
	group->add(customPlayHeight_);
	group->add(portBox_);
	group->add(button);

	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initCreateClientFrame() {
	multiFrame_.setCurrentFrame(createClientFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr header = gui::createTextItem("Client", fontDefault, hDistance_, textColor_);

	gui::ButtonPtr menu = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	menu->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});
	gui::ButtonPtr b1 = createButton("Server", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(createServerFrameIndex_);
	});

	multiFrame_.add(menu,0,0,false,true);
	multiFrame_.add(header,0,50,false,true);
	multiFrame_.add(b1,80,0,false,true);

	auto manButton = createManButton(hDistance_, spriteMan, spriteCross);
	manButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfHumanPlayers(nbr+1);
		int newNbr = getNbrOfHumanPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfHumanPlayers(newNbr);
	});

	auto computerButton = createManButton(hDistance_, spriteComputer, spriteCross);
	computerButton->setNbr(0);
	computerButton->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNbrOfComputerPlayers(nbr+1);
		int newNbr = getNbrOfComputerPlayers();

		if (newNbr == nbr) {
            newNbr = 0;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNbrOfComputerPlayers(newNbr);
	});

    gui::TextItemPtr textItem = gui::createTextItem("Connect to ip", fontDefault18, 18, textColor_);
    ipBox_ = createTextBox(260);
	ipBox_->setInputFormatter(std::make_shared<gui::InputFormatter>(15));
	ipBox_->setText("127.0.0.1");
	multiFrame_.add(textItem,0,100,false,true);
	multiFrame_.add(ipBox_,120,100,false,true);

	gui::TextItemPtr textItem3 = gui::createTextItem("Port", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem3,0,150,false,true);
	multiFrame_.add(portBox_,55,150,false,true);

	gui::TextItemPtr textItem4 = gui::createTextItem("Local players:", fontDefault18, 18, textColor_);
	multiFrame_.add(textItem4,0,200,false,true);
	multiFrame_.add(manButton,120,200,false,true);
	multiFrame_.add(computerButton,260,200,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Join", 30, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(waitToConnectFrameIndex_);
		std::stringstream stream;
		stream << portBox_->getText();
		int port;
		stream >> port;
		createClientGame(port,ipBox_->getText());
		resumeButton_->setVisible(false);
	});

	multiFrame_.add(button,0,250,false,true);

	// Add all items to group!
	gui::GroupPtr group = gui::createGroup(SDLK_UP,SDLK_DOWN);
	group->add(menu);
	group->add(b1);
	group->add(ipBox_);
	group->add(portBox_);
	group->add(button);

	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initWaitToConnectFrame() {
	multiFrame_.setCurrentFrame(waitToConnectFrameIndex_);

	gui::ButtonPtr menu = createButton("Abort connection", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});

	multiFrame_.add(menu,0,0,false,true);

	gui::TextItemPtr textItem = gui::createTextItem("Waiting for the server to accept connection!", fontDefault18,18, textColor_);

	multiFrame_.add(textItem,0,100,false,true);
}

void GuiWindow::initNewHighScoreFrame() {
	multiFrame_.setCurrentFrame(newHighscoreFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr textItem = gui::createTextItem("Name", fontDefault18,18, textColor_);
	nameBox_ = createTextBox(250);
	nameBox_->setInputFormatter(std::make_shared<gui::InputFormatter>(7));
	nameBox_->setFocus(true);

	multiFrame_.add(textItem,45,50,false,true);
	multiFrame_.add(nameBox_,100,50,false,true);

	gui::ButtonPtr b1 = createButton("Done!", hDistance_, [&](gui::GuiItem*) {
		std::string name = nameBox_->getText();
		if (name.size() > 0) {
			std::time_t t = std::time(NULL);
			char mbstr[30];
			std::strftime(mbstr, 30, "%Y-%m-%d", std::localtime(&t));
            highscorePtr_->addNewRecord(name,mbstr);
            multiFrame_.setCurrentFrame(highscoreFrameIndex_);
			saveHighscore();
		}
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
				item->click();
				break;
			}
		}
	});

	multiFrame_.add(b1,45,100,false,true);

	gui::GroupPtr group = gui::createGroup(SDLK_UP,SDLK_DOWN);
	group->add(nameBox_);
	group->add(b1);
	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initNetworkPlayFrame() {
	multiFrame_.setCurrentFrame(networkPlayFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::GuiItem* item) {
		multiFrame_.setCurrentFrame(0);
		item->setFocus(false);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});

	gui::ButtonPtr b2 = createButton("Restart", hDistance_, [&](gui::GuiItem* item) {
		restartGame();
		item->setFocus(false);
	});
	b2->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == restartKey_) {
				item->click();
				break;
			}
		}
	});

	multiFrame_.add(b1,0,0,false,true);
	multiFrame_.add(b2,80,0,false,true);
	multiFrame_.add(pause_,0,0,true,true);
}

void GuiWindow::initAiFrame() {
	multiFrame_.setCurrentFrame(aiFrameIndex_);
	
	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});
	b1->addSdlEventListener([&](gui::GuiItem* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
		case SDL_QUIT:
			item->click();
			break;
		case SDL_KEYDOWN:
			SDLKey key = sdlEvent.key.keysym.sym;
			if (key == SDLK_ESCAPE) {
				item->click();
				break;
			}
		}
	});
	gui::ButtonPtr b2 = createButton("Option", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(optionFrameIndex_);
	});

	aiText1_ = gui::createTextItem("Ai1:", fontDefault18, 18, textColor_);
	aiText2_ = gui::createTextItem("Ai2:", fontDefault18, 18, textColor_);
	aiText3_ = gui::createTextItem("Ai3:", fontDefault18, 18, textColor_);
	aiText4_ = gui::createTextItem("Ai4:", fontDefault18, 18, textColor_);

	// Load all specific ai settings.
	loadAllSettings();

	gui::TextButtonPtr ai1Button = createButton(ai1_.getName(), hDistance_, [&](gui::GuiItem* item) {
		gui::TextButton* button = (gui::TextButton*) item;
		int index = -1;
		
		// Ai found?
		for (unsigned int i = 0; i < ais_.size(); ++i) {
			if (ais_[i].getName() == button->getText()) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			// Next ai in vector.
			ai1_ = ais_[(index+1)%ais_.size()];
			button->setText(ai1_.getName());
		} else {
			// Default ai.
			ai1_ = Ai();
		}
		saveAllSettings();
	});

	gui::TextButtonPtr ai2Button = createButton(ai2_.getName(), hDistance_, [&](gui::GuiItem* item) {
		gui::TextButton* button = (gui::TextButton*) item;
		int index = -1;
		
		// Ai found?
		for (unsigned int i = 0; i < ais_.size(); ++i) {
			if (ais_[i].getName() == button->getText()) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			// Next ai in vector.
			ai2_ = ais_[(index+1)%ais_.size()];
			button->setText(ai2_.getName());
		} else {
			// Default ai.
			ai2_ = Ai();
		}
		saveAllSettings();
	});

	gui::TextButtonPtr ai3Button = createButton(ai3_.getName(), hDistance_, [&](gui::GuiItem* item) {
		gui::TextButton* button = (gui::TextButton*) item;
		int index = -1;
		
		// Ai found?
		for (unsigned int i = 0; i < ais_.size(); ++i) {
			if (ais_[i].getName() == button->getText()) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			// Next ai in vector.
			ai3_ = ais_[(index+1)%ais_.size()];
			button->setText(ai3_.getName());
		} else {
			// Default ai.
			ai3_ = Ai();
		}
		saveAllSettings();
	});

	gui::TextButtonPtr ai4Button = createButton(ai4_.getName(), hDistance_, [&](gui::GuiItem* item) {
		gui::TextButton* button = (gui::TextButton*) item;
		int index = -1;
		
		// Ai found?
		for (unsigned int i = 0; i < ais_.size(); ++i) {
			if (ais_[i].getName() == button->getText()) {
				index = i;
				break;
			}
		}

		if (index != -1) {
			// Next ai in vector.
			ai4_ = ais_[(index+1)%ais_.size()];
			button->setText(ai4_.getName());
		} else {
			// Default ai.
			ai4_ = Ai();
		}
		saveAllSettings();
	});

	gui::ButtonPtr reloadAis = createButton("Reload Ai:s", hDistance_, [&](gui::GuiItem*) {
		loadAllSettings();
		bool ai1 = false, ai2 = false, ai3 = false, ai4 = false;
		for (const Ai& ai : ais_) {
			if (ai.getName() == ai1Button->getText()) {
				ai1 = true;
			}
			if (ai.getName() == ai2Button->getText()) {
				ai2 = true;
			}
			if (ai.getName() == ai3Button->getText()) {
				ai3 = true;
			}
			if (ai.getName() == ai4Button->getText()) {
				ai4 = true;
			}
		}
		if (!ai1) {
			// Set to default ai.
			ai1_ = Ai();
			ai1Button->setText(ai1_.getName());
		}
		if (!ai2) {
			// Set to default ai.
			ai2_ = Ai();
			ai1Button->setText(ai2_.getName());
		}
		if (!ai3) {
			// Set to default ai.
			ai3_ = Ai();
			ai1Button->setText(ai3_.getName());
		}
		if (!ai4) {
			// Set to default ai.
			ai4_ = Ai();
			ai1Button->setText(ai4_.getName());
		}
	});

	multiFrame_.add(b1, 0, 0, false, true);
	multiFrame_.add(b2, 80, 0, false, true);
	multiFrame_.add(aiText1_, 10, hDistance_ + 10, false, true);
	multiFrame_.add(aiText2_, 10, hDistance_ + 60, false, true);
	multiFrame_.add(aiText3_, 10, hDistance_ + 110, false, true);
	multiFrame_.add(aiText4_, 10, hDistance_ + 160, false, true);
	multiFrame_.add(reloadAis, 10, hDistance_ + 210, false, true);

	multiFrame_.add(ai1Button, 60, hDistance_ + 10, false, true);
	multiFrame_.add(ai2Button, 60, hDistance_ + 60, false, true);
	multiFrame_.add(ai3Button, 60, hDistance_ + 110, false, true);
	multiFrame_.add(ai4Button, 60, hDistance_ + 160, false, true);
}

void GuiWindow::loadAllSettings() {
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
	// Load ais to ai1, ... and ai4.
	{
		std::ifstream file("settings");
		if (file.is_open()) {
			std::string filename;
			// Load ai1.
			std::getline(file, filename);
			loadAi(ai1_, "ais/", filename);
			// Load ai2.
			std::getline(file, filename);
			loadAi(ai2_, "ais/", filename);
			// Load ai3.
			std::getline(file, filename);
			loadAi(ai3_, "ais/", filename);
			// Load ai4.
			std::getline(file, filename);
			loadAi(ai4_, "ais/", filename);
		}
	}
}

void GuiWindow::saveAllSettings() {
	std::ofstream file("settings");
	if (file.is_open() && file.good()) {
		file << ai1_.getName() << "\n";
		file << ai2_.getName() << "\n";
		file << ai3_.getName() << "\n";
		file << ai4_.getName() << "\n";;
	}
}

void GuiWindow::resize(int width, int height) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0,0,width,height);
	glOrtho(0,width,0,height,-1,1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

// Override mw::Window
void GuiWindow::update(Uint32 deltaTime) {
	multiFrame_.draw(deltaTime/1000.0);
	if (isUpdatingGame()) {
		drawGame(deltaTime);
	}
	updateGame(deltaTime);
}

// Override mw::Window
void GuiWindow::eventUpdate(const SDL_Event& windowEvent) {
	multiFrame_.eventUpdate(windowEvent);
	switch (windowEvent.type) {
	case SDL_KEYDOWN:
		switch (windowEvent.key.keysym.sym) {
		case SDLK_F11:
			mw::Window::setFullScreen(!mw::Window::isFullScreen());
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
	if (isUpdatingGame()) {
		updateGameEvent(windowEvent);
	}
}
