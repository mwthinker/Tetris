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

#include "gamesprite.h"
#include "gamefont.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <iostream>
#include <string>
#include <ctime>

GuiWindow::GuiWindow() : mw::Window(520,640,"MWetris","images/tetris.bmp") {
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

	auto background = gui::createImageBackground(spriteBackground);
	multiFrame_.setBackground(background,0);
	multiFrame_.setBackground(background,networkFrameIndex_);
	multiFrame_.setBackground(background,playFrameIndex_);
	multiFrame_.setBackground(background,highscoreFrameIndex_);
	multiFrame_.setBackground(background,customFrameIndex_);
	multiFrame_.setBackground(background,optionFrameIndex_);
	multiFrame_.setBackground(background,newHighscoreFrameIndex_);
	multiFrame_.setBackground(background,createClientFrameIndex_);
	multiFrame_.setBackground(background,createServerFrameIndex_);
	multiFrame_.setBackground(background,loobyClientFrameIndex_);
	multiFrame_.setBackground(background,loobyServerFrameIndex_);
	multiFrame_.setBackground(background,waitToConnectFrameIndex_);	
	multiFrame_.setBackground(background,networkPlayFrameIndex_);	

	hDistance_ = 30;

	initFrameMenu();
	initHighscoreFrame();
	initPlayFrame();
	initCustomPlayFrame();
	initOptionFrame();
	initNewHighScoreFrame();
	initCreateServerFrame();
	initCreateClientFrame();
	initServerLoobyFrame();
	initClientLoobyFrame();
	initWaitToConnectFrame();
	initNetworkPlayFrame();

	multiFrame_.setCurrentFrame(0);
	mw::Window::setUnicodeInputEnable(true);
	resize(getWidth(),getHeight());

	pauseKey_ = SDLK_p;
	restartKey_ = SDLK_F2;
}

void GuiWindow::quit() {
	setQuiting(true);
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

gui::TextButtonPtr GuiWindow::createButton(std::string text, int size, std::function<void(gui::GuiItem*)> onClick) {
	mw::Color textColor(1.0,0.1,0.1);
	mw::Color focus(0.8, 0.1, 0, 0.3);
	mw::Color onHover(0.6, 0.1, 0.1);
	mw::Color notHover(0.4, 0.0, 0.0,0.0);
	mw::Color pushed(0.8, 0.0, 0, 0.7);
	gui::TextButtonPtr button = gui::createTextButton(text, size, fontDefault,
		textColor,focus,onHover,notHover,pushed);
	button->addOnClickListener(onClick);
	return button;
}

gui::TextBoxPtr GuiWindow::createTextBox(int size) {
	mw::Color textColor(1.0,1.0,1.0);
	mw::Color focus(0.8, 0.1, 0, 0.3);
	mw::Color onHover(0.6, 0.1, 0.1);
	return gui::createTextBoxDraw(size, fontDefault, 30, textColor, focus, onHover);
}

gui::BarColorPtr GuiWindow::createUpperBar() {
	return gui::createBarColor(gui::Bar::UP,hDistance_,mw::Color(0.5,0,0,0.30));
}

void GuiWindow::initFrameMenu() {
	multiFrame_.setCurrentFrame(0);
	multiFrame_.add(gui::createTextItem("MWetris",fontDefault50,80,mw::Color(1,1,1)),10,50,false,true);//Color(0.9,0.2,0,0.9)
	multiFrame_.add(gui::createTextItem("Made by Marcus Welander",fontDefault18,12,mw::Color(1,1,1)),0,0,true,false);

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
		multiFrame_.setCurrentFrame(playFrameIndex_);
		restartGame();
	});

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
	group->add(b1);
	group->add(b2);
	group->add(b3);
	group->add(b7);
	group->add(b4);
	group->add(b5);
	group->add(b6);

	multiFrame_.add(group,0,0,false,false);
}

void GuiWindow::initPlayFrame() {
	multiFrame_.setCurrentFrame(playFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::GuiItem* item) {
		multiFrame_.setCurrentFrame(0);
		item->setFocus(false);
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

	manButton_ = createManButton(hDistance_);
	manButton_->addOnClickListener([&](gui::GuiItem* item) {
		ManButton* nbrOfPlayers = (ManButton*) item;
		int nbr = nbrOfPlayers->getNbr();
		setNumberOfLocalPlayers(nbr+1);
		int newNbr = getNumberOfLocalPlayers();

		if (newNbr == nbr) {
            newNbr = 1;
		}

		nbrOfPlayers->setNbr(newNbr);
		setNumberOfLocalPlayers(newNbr);

		restartGame();
	});

	pause_ = createButton("Pause", hDistance_, [&](gui::GuiItem* item) {
		setPause(!isPaused());
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
	multiFrame_.add(manButton_,80 + b2->getWidth(),0,false,true);
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

	highscorePtr_ = createHighscore(10,mw::Color(1,1,1));
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

	multiFrame_.add(menu,0,0,false,true);

	// Set board size ------------------------------------------------------
	gui::TextItemPtr textItem = gui::createTextItem("Width",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem,45,50,false,true);
	customPlayWidth_ = createTextBox(35);
	customPlayWidth_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	customPlayWidth_->setText("10");
	multiFrame_.add(customPlayWidth_,100,50,false,true);

	gui::TextItemPtr textItem2 = gui::createTextItem("Height",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem2,140,50,false,true);
	customPlayHeight_ = createTextBox(35);
	customPlayHeight_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	customPlayHeight_->setText("10");
	multiFrame_.add(customPlayHeight_,200,50,false,true);

	// Set max level -----------------------------------------------------
	gui::TextItemPtr textItem3 = gui::createTextItem("Max Level",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem3,45,100,false,true);
	customPlaymaxLevel_ = createTextBox(35);
	customPlaymaxLevel_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(2));
	customPlaymaxLevel_->setText("20");
	multiFrame_.add(customPlaymaxLevel_,140,100,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Create game", 30, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(playFrameIndex_);
		std::stringstream stream;
		stream << customPlayWidth_->getText() << " ";
		stream << customPlayHeight_->getText() << " ";
		stream << customPlaymaxLevel_->getText() << " ";
		int width, height, maxLevel;
		stream >> width >> height >> maxLevel;
		createCustomGame(width,height,maxLevel);
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

void GuiWindow::initOptionFrame() {
	multiFrame_.setCurrentFrame(optionFrameIndex_);

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

	multiFrame_.add(b1,0,0,false,true);
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
		setReady(!isReady());
	});

	gui::ButtonPtr b3 = createButton("Start", hDistance_, [&](gui::GuiItem* item) {
		restartGame();
		multiFrame_.setCurrentFrame(networkPlayFrameIndex_);
	});

	multiFrame_.add(b1,0,0,false,true);

	networkLoobyPtr_ = createNetworkLooby();

	multiFrame_.add(b1,0,0,false,true);
	multiFrame_.add(networkLoobyPtr_, 0, 100, false,true);
	multiFrame_.add(ready_,0,400,false,true);
	multiFrame_.add(b3,100,400,false,true);
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
}

void GuiWindow::initCreateServerFrame() {
	multiFrame_.setCurrentFrame(createServerFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr header = gui::createTextItem("Server",fontDefault,30,mw::Color(1.0,1.0,1.0));

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

	// Set board size ------------------------------------------------------
	gui::TextItemPtr textItem = gui::createTextItem("Width",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem,0,100,false,true);
	multiFrame_.add(customPlayWidth_,55,100,false,true);

	gui::TextItemPtr textItem2 = gui::createTextItem("Height",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem2,95,100,false,true);
	multiFrame_.add(customPlayHeight_,155,100,false,true);

	gui::TextItemPtr textItem3 = gui::createTextItem("Port",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	portBox_ = createTextBox(105);
	portBox_->setInputFormatter(std::make_shared<gui::InputNumberFormatter>(6));
	portBox_->setText("11155");
	multiFrame_.add(textItem3,0,150,false,true);
	multiFrame_.add(portBox_,55,150,false,true);

	gui::TextItemPtr textItem4 = gui::createTextItem("Local players:",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem4,0,200,false,true);
	multiFrame_.add(manButton_,120,200,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Connect", 30, [&](gui::GuiItem*) {
		std::stringstream stream;
		stream << customPlayWidth_->getText() << " ";
		stream << customPlayHeight_->getText() << " ";
		stream << portBox_->getText();
		int width, height, port;
		stream >> width >> height >> port;
		createServerGame(port);
		resumeButton_->setVisible(true);
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

	gui::TextItemPtr header = gui::createTextItem("Client",fontDefault,30,mw::Color(1.0,1.0,1.0));

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

    gui::TextItemPtr textItem = gui::createTextItem("Connect to ip",fontDefault18,18,mw::Color(1.0,1.0,1.0));
    ipBox_ = createTextBox(260);
	ipBox_->setInputFormatter(std::make_shared<gui::InputFormatter>(15));
	multiFrame_.add(textItem,0,100,false,true);
	multiFrame_.add(ipBox_,120,100,false,true);

	gui::TextItemPtr textItem3 = gui::createTextItem("Port",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem3,0,150,false,true);
	multiFrame_.add(portBox_,55,150,false,true);

	gui::TextItemPtr textItem4 = gui::createTextItem("Local players:",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	multiFrame_.add(textItem4,0,200,false,true);
	multiFrame_.add(manButton_,120,200,false,true);

	// Create game -----------------------------------------------------
	gui::ButtonPtr button = createButton("Connect", 30, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(playFrameIndex_);
		std::stringstream stream;
		stream << customPlayWidth_->getText() << " ";
		stream << customPlayHeight_->getText() << " ";
		int width, height;
		stream >> width >> height;
		createCustomGame(width,height,20);
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
	gui::ButtonPtr menu = createButton("Abort connection", hDistance_, [&](gui::GuiItem*) {
		multiFrame_.setCurrentFrame(0);
	});

	multiFrame_.add(menu,0,0,false,true);

	gui::TextItemPtr textItem = gui::createTextItem("Waiting for the server to accept connection!",fontDefault18,18,mw::Color(1.0,1.0,1.0));

	multiFrame_.add(menu,0,100,false,true);
}

void GuiWindow::initNewHighScoreFrame() {
	multiFrame_.setCurrentFrame(newHighscoreFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr textItem = gui::createTextItem("Name",fontDefault18,18,mw::Color(1.0,1.0,1.0));
	nameBox_ = createTextBox(250);
	nameBox_->setInputFormatter(std::make_shared<gui::InputFormatter>(7));
	nameBox_->setFocus(true);

	multiFrame_.add(textItem,45,50,false,true);
	multiFrame_.add(nameBox_,100,50,false,true);

	gui::ButtonPtr b1 = createButton("Done!", 30, [&](gui::GuiItem*) {
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
		updateGame(deltaTime);
	}
}

// Override mw::Window
void GuiWindow::eventUpdate(const SDL_Event& windowEvent) {
	multiFrame_.eventUpdate(windowEvent);
	switch (windowEvent.type) {
	case SDL_KEYDOWN:
		switch (windowEvent.key.keysym.sym) {
		case SDLK_f:
			// Fall through!
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
