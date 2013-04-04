#include "guiwindow.h"

#include "barcolor.h"
#include "imagebackground.h"
#include "textbutton.h"
#include "group.h"
#include "multiframe.h"
#include "textitem.h"
#include "textboxdraw.h"
#include "inputnumberformatter.h"

#include "../gamesprite.h"
#include "../gamefont.h"

#include <mw/font.h>
#include <mw/sprite.h>
#include <mw/window.h>

#include <iostream>
#include <string>

namespace gui {

	GuiWindow::GuiWindow() : mw::Window(500,600,"MWetris","images/tetris.bmp") {
		playFrameIndex_ = multiFrame_.addFrameBack();
		highscoreFrameIndex_ = multiFrame_.addFrameBack();
		customFrameIndex_ = multiFrame_.addFrameBack();
		optionFrameIndex_ = multiFrame_.addFrameBack();		

		auto background = std::make_shared<ImageBackground>(spriteBackground);
		multiFrame_.setBackground(background,0);
		multiFrame_.setBackground(background,playFrameIndex_);
		multiFrame_.setBackground(background,highscoreFrameIndex_);
		multiFrame_.setBackground(background,customFrameIndex_);
		multiFrame_.setBackground(background,optionFrameIndex_);

		hDistance_ = 30;

		initFrameMenu();
		initHighscoreFrame();
		initPlayFrame();
		initCustomPlayFrame();
		initOptionFrame();

		multiFrame_.setCurrentFrame(0);
		mw::Window::setUnicodeInputEnable(true);
	}

	void GuiWindow::test3() {
		std::cout << "3\n";
	}

	void GuiWindow::test2() {
		std::cout << "2\n";
	}

	void GuiWindow::test1() {
		std::cout << "1\n";
	}

	void GuiWindow::quit() {
		setQuiting(true);
	}

	ButtonPtr GuiWindow::createTextButton(std::string text, int size, std::function<void()> onClick) {
		Color textColor(1.0,0.1,0.1);
		Color focus(0.8, 0.1, 0, 0.3);
		Color onHover(0.6, 0.1, 0.1);
		Color notHover(0.4, 0.0, 0.0,0.0);
		Color pushed(0.8, 0.0, 0, 0.7);
		ButtonPtr button(new TextButton(text, size, fontDefault,
			textColor,focus,onHover,notHover,pushed));
		button->addOnClickListener(onClick);
		return button;
	}

	TextBoxPtr GuiWindow::createTextBox(int size) {
		Color textColor(1.0,1.0,1.0);
		Color focus(0.8, 0.1, 0, 0.3);
		Color onHover(0.6, 0.1, 0.1);
		return TextBoxPtr(new TextBoxDraw(size, fontDefault, 30, textColor, focus, onHover));
	}

	void GuiWindow::initFrameMenu() {
		multiFrame_.setCurrentFrame(0);
		multiFrame_.add(std::make_shared<TextItem>("MWetris",fontDefault50,80,Color(0.9,0.2,0,0.9)),10,50,false,true);
		multiFrame_.add(std::make_shared<TextItem>("Made by Marcus Welander",fontDefault18,12,Color(0.9,0.2,0,0.9)),0,0,true,false);

		// Upper bar.		
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		// Text is outside due to invY which dont know where the top of the text begin.
		// The bottom is put where it is supposed to but the invers not.
		ButtonPtr b1 = createTextButton("[Resume]", hDistance_, std::bind(&GuiWindow::test1,this));
		multiFrame_.add(b1,0,0,false,true);

		auto playFrame = [&]() {
			multiFrame_.setCurrentFrame(playFrameIndex_);
		};

		// Menu.
		ButtonPtr b2 = createTextButton("[Play]", 35, std::bind(&GuiWindow::test1,this));
		b2->addOnClickListener([&]() {
			multiFrame_.setCurrentFrame(playFrameIndex_);
		});
		ButtonPtr b3 = createTextButton("[Custom play]", 35, std::bind(&GuiWindow::test2,this));
		b3->addOnClickListener([&]() {
			multiFrame_.setCurrentFrame(customFrameIndex_);
		});
		ButtonPtr b4 = createTextButton("[Highscore]", 35, std::bind(&GuiWindow::test2,this));
		b4->addOnClickListener([&]() {
			multiFrame_.setCurrentFrame(highscoreFrameIndex_);
		});		
		ButtonPtr b5 = createTextButton("[Options]", 35, std::bind(&GuiWindow::test3,this));
		b5->addOnClickListener([&]() {
			multiFrame_.setCurrentFrame(optionFrameIndex_);
		});
		ButtonPtr b6 = createTextButton("[Exit]", 35, std::bind(&GuiWindow::quit,this));

		int x = 10;
		int y = 150;
		int distance = 50;

		multiFrame_.add(b2,x,y,false,true);
		y += distance;
		multiFrame_.add(b3,x,y,false,true);
		y += distance;
		multiFrame_.add(b4,x,y,false,true);
		y += distance;
		multiFrame_.add(b5,x,y,false,true);
		y += distance;
		multiFrame_.add(b6,x,y,false,true);
		auto textBox = createTextBox(100);
		multiFrame_.add(textBox,x+50,y+50,false,true);

		// Focus is switchable by the left and right arrow.
		GroupPtr group = std::make_shared<Group>(SDLK_UP,SDLK_DOWN);
		group->add(b1);
		group->add(b2);
		group->add(b3);
		group->add(b4);
		group->add(b5);
		group->add(b6);
		group->add(textBox);

		multiFrame_.add(group,0,0,false,false);
	}

	void GuiWindow::initPlayFrame() {
		multiFrame_.setCurrentFrame(playFrameIndex_);

		// Upper bar.
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		ButtonPtr b1 = createTextButton("[Menu]", hDistance_, std::bind(&GuiWindow::test1,this));
		b1->addOnClickListener([&]() {
			multiFrame_.setCurrentFrame(0);
		});

		ButtonPtr b2 = createTextButton("[Restart]", hDistance_, std::bind(&GuiWindow::restartGame,this));
		ButtonPtr b3 = createTextButton("[Pause]", hDistance_, std::bind(&GuiWindow::pauseGame,this));

		multiFrame_.add(b1,0,0,false,true);
		multiFrame_.add(b2,100,0,false,true);
		multiFrame_.add(b3,0,0,true,true);
	}

	void GuiWindow::initHighscoreFrame() {
		multiFrame_.setCurrentFrame(highscoreFrameIndex_);

		// Upper bar.
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		ButtonPtr b1 = createTextButton("[Menu]", hDistance_, [&]() {
			multiFrame_.setCurrentFrame(0);
		});

		multiFrame_.add(b1,0,0,false,true);
	}

	void GuiWindow::initCustomPlayFrame() {
		multiFrame_.setCurrentFrame(customFrameIndex_);

		// Upper bar.
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		ButtonPtr menu = createTextButton("[Menu]", hDistance_, [&]() {
			multiFrame_.setCurrentFrame(0);
		});

		multiFrame_.add(menu,0,0,false,true);

		// Set board size ------------------------------------------------------
		TextItemPtr textItem(new TextItem("Width",fontDefault18,18,Color(1.0,1.0,1.0)));
		multiFrame_.add(textItem,45,50,false,true);
		customPlayWidth_ = createTextBox(35);		
		customPlayWidth_->setInputFormatter(std::make_shared<InputNumberFormatter>(2));
		customPlayWidth_->setText("10");
		multiFrame_.add(customPlayWidth_,100,50,false,true);

		TextItemPtr textItem2(new TextItem("Height",fontDefault18,18,Color(1.0,1.0,1.0)));
		multiFrame_.add(textItem2,140,50,false,true);
		customPlayHeight_ = createTextBox(35);
		customPlayHeight_->setInputFormatter(std::make_shared<InputNumberFormatter>(2));
		customPlayHeight_->setText("10");
		multiFrame_.add(customPlayHeight_,200,50,false,true);

		// Set max level -----------------------------------------------------
		TextItemPtr textItem3(new TextItem("Max Level",fontDefault18,18,Color(1.0,1.0,1.0)));
		multiFrame_.add(textItem3,45,100,false,true);
		customPlaymaxLevel_ = createTextBox(35);
		customPlaymaxLevel_->setInputFormatter(std::make_shared<InputNumberFormatter>(2));
		customPlaymaxLevel_->setText("20");
		multiFrame_.add(customPlaymaxLevel_,140,100,false,true);

		// Create game -----------------------------------------------------
		auto button = createTextButton("Create game", 30, [&]() {
			multiFrame_.setCurrentFrame(playFrameIndex_);
			std::stringstream stream;
			stream << customPlayWidth_->getText() << " ";
			stream << customPlayHeight_->getText() << " ";
			stream << customPlaymaxLevel_->getText() << " ";
			int width, height, maxLevel;
			stream >> width >> height >> maxLevel;
			createCustomGame(width,height,maxLevel);
		});

		multiFrame_.add(button,45,150,false,true);

		// Add all items to group!
		GroupPtr group = std::make_shared<Group>(SDLK_UP,SDLK_DOWN);
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
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		ButtonPtr b1 = createTextButton("[Menu]", hDistance_, [&]() {
			multiFrame_.setCurrentFrame(0);
		});

		multiFrame_.add(b1,0,0,false,true);
	}

	void GuiWindow::initCreateClientFrame() {
		multiFrame_.setCurrentFrame(optionFrameIndex_);

		// Upper bar.
		multiFrame_.addBar(std::make_shared<BarColor>(Bar::UP,hDistance_,Color(0.5,0,0,0.30)));

		ButtonPtr b1 = createTextButton("[Menu]", hDistance_, [&]() {
			multiFrame_.setCurrentFrame(0);
		});
	}

	void GuiWindow::resize(int width, int height) {
	}

	// Override mw::Window
	void GuiWindow::update(Uint32 deltaTime) {
		multiFrame_.draw(deltaTime/1000.0);
		if (false) {
			updateGame(deltaTime);
		}
	}

	// Override mw::Window
	void GuiWindow::eventUpdate(const SDL_Event& windowEvent) {
		multiFrame_.eventUpdate(windowEvent);
		if (false) {
			updateGameEvent(windowEvent);
		}

		switch (windowEvent.type) {
		case SDL_QUIT:
			mw::Window::setQuiting(true);
			break;
		case SDL_KEYDOWN:
			switch (windowEvent.key.keysym.sym) {
			case SDLK_p:
			case SDLK_PAUSE:
				break;
			case SDLK_ESCAPE:
				mw::Window::setQuiting(true);
				break;
			case SDLK_a:
				break;
			default:
				break;
			}
			break;
		default:
			break;
		}
	}

} // Namespace gui.