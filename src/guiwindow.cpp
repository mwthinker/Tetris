/*

GuiWindow::GuiWindow() : mw::Window(520, 640, "MWetris", "images/tetris.bmp"), multiFrame_(mw::Window::getSdlWindow()) {
	// Set colors.
	textColor_ = mw::Color(1, 1, 1);
	textButtonColor_ = mw::Color(1, .1, .1);
	focusColor_ = mw::Color(.8, .1, 0, .3);
	onHoverColor_ = mw::Color(.6, .1, .1);
	notHoverColor_ = mw::Color(.4, .0, .0, .0);
	pushedColor_ = mw::Color(.8, .0, 0, .7);
	barColor_ = mw::Color(.5, 0, 0, .30);
	hDistance_ = 30;	
}

gui::TextButtonPtr GuiWindow::createButton(std::string text, int size, std::function<void(gui::Widget*)> onClick) {
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

void GuiWindow::initOptionFrame(const std::vector<DevicePtr>& devices) {
	// Upper bar.
	multiFrame_.addBar(createUpperBar(), optionFrameIndex_);

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(0);
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					item->click();
					break;
				}
		}
	});

	gui::ButtonPtr b2 = createButton("AI", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(aiFrameIndex_);
	});

	gui::TextItemPtr textItem1 = gui::createTextItem("Connected players", fontDefault, 30, textColor_);
	multiFrame_.add(textItem1, 0, hDistance_, false, true, optionFrameIndex_);
	int size = devices.size();
	for (int i = 0; i < size; ++i) {
		std::stringstream stream;
		stream << "Player " << 1 + i << ": " << devices[i]->getName();
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

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					item->click();
					break;
				}
		}
	});

	ready_ = createButton("Ready", hDistance_, [&](gui::Widget* item) {
		changeReadyState();
	});

	gui::ButtonPtr b2 = createButton("Start", hDistance_, [&](gui::Widget* item) {
		startGame();
	});

	networkLoobyPtr_ = createNetworkLooby();

	multiFrame_.add(b1, 0, 0, false, true);
	multiFrame_.add(networkLoobyPtr_, 0, 100, false, true);
	multiFrame_.add(ready_, 0, 400, false, true);
	multiFrame_.add(b2, 100, 400, false, true);
}

void GuiWindow::initClientLoobyFrame() {
	multiFrame_.setCurrentFrame(loobyClientFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					item->click();
					break;
				}
		}
	});

	multiFrame_.add(b1, 0, 0, false, true);
	multiFrame_.add(networkLoobyPtr_, 0, 100, false, true);
	multiFrame_.add(ready_, 0, 400, false, true);
}

// Create game -----------------------------------------------------
gui::ButtonPtr button = createButton("Create", 30, [&](gui::Widget*) {
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

void GuiWindow::initWaitToConnectFrame() {
	multiFrame_.setCurrentFrame(waitToConnectFrameIndex_);

	gui::ButtonPtr menu = createButton("Abort connection", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(0);
		abortGame();
	});

	multiFrame_.add(menu, 0, 0, false, true);

	gui::TextItemPtr textItem = gui::createTextItem("Waiting for the server to accept connection!", fontDefault18, 18, textColor_);

	multiFrame_.add(textItem, 0, 100, false, true);
}

void GuiWindow::initNewHighScoreFrame() {
	multiFrame_.setCurrentFrame(newHighscoreFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::TextItemPtr textItem = gui::createTextItem("Name", fontDefault18, 18, textColor_);
	nameBox_ = createTextBox(250);
	nameBox_->setInputFormatter(std::make_shared<gui::InputFormatter>(7));
	nameBox_->setFocus(true);

	multiFrame_.add(textItem, 45, 50, false, true);
	multiFrame_.add(nameBox_, 100, 50, false, true);

	gui::ButtonPtr b1 = createButton("Done!", hDistance_, [&](gui::Widget*) {
		std::string name = nameBox_->getText();
		if (name.size() > 0) {
			std::time_t t = std::time(NULL);
			char mbstr[30];
			std::strftime(mbstr, 30, "%Y-%m-%d", std::localtime(&t));
			highscorePtr_->addNewRecord(name, mbstr);
			multiFrame_.setCurrentFrame(highscoreFrameIndex_);
			saveHighscore();
		}
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_RETURN || key == SDLK_KP_ENTER) {
					item->click();
					break;
				}
		}
	});
}

void GuiWindow::initNetworkPlayFrame() {
	multiFrame_.setCurrentFrame(networkPlayFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Abort", hDistance_, [&](gui::Widget* item) {
		multiFrame_.setCurrentFrame(0);
		item->setFocus(false);
		abortGame();
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					item->click();
					break;
				}
		}
	});

	gui::ButtonPtr b2 = createButton("Restart", hDistance_, [&](gui::Widget* item) {
		restartGame();
		item->setFocus(false);
	});
	b2->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == restartKey_) {
					item->click();
					break;
				}
		}
	});

void GuiWindow::initAiFrame() {
	multiFrame_.setCurrentFrame(aiFrameIndex_);

	// Upper bar.
	multiFrame_.addBar(createUpperBar());

	gui::ButtonPtr b1 = createButton("Menu", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(0);
	});
	b1->addSdlEventListener([&](gui::Widget* item, const SDL_Event& sdlEvent) {
		switch (sdlEvent.type) {
			case SDL_QUIT:
				item->click();
				break;
			case SDL_KEYDOWN:
				SDL_Keycode key = sdlEvent.key.keysym.sym;
				if (key == SDLK_ESCAPE) {
					item->click();
					break;
				}
		}
	});
	gui::ButtonPtr b2 = createButton("Option", hDistance_, [&](gui::Widget*) {
		multiFrame_.setCurrentFrame(optionFrameIndex_);
	});

	gui::TextItemPtr aiText1 = gui::createTextItem("Ai1:", fontDefault18, 18, textColor_);
	gui::TextItemPtr aiText2 = gui::createTextItem("Ai2:", fontDefault18, 18, textColor_);
	gui::TextItemPtr aiText3 = gui::createTextItem("Ai3:", fontDefault18, 18, textColor_);
	gui::TextItemPtr aiText4 = gui::createTextItem("Ai4:", fontDefault18, 18, textColor_);

	// Load all specific ai settings.
	loadAllSettings();

	ai1Button_ = createButton(activeAis_[0].getName(), hDistance_, [&](gui::Widget* item) {
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
			activeAis_[0] = ais_[(index + 1) % ais_.size()];
			button->setText(activeAis_[0].getName());
		}
		else {
			// Default ai.
			activeAis_[0] = Ai();
		}
		saveAllSettings();
	});

	ai2Button_ = createButton(activeAis_[1].getName(), hDistance_, [&](gui::Widget* item) {
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
			activeAis_[1] = ais_[(index + 1) % ais_.size()];
			button->setText(activeAis_[1].getName());
		}
		else {
			// Default ai.
			activeAis_[1] = Ai();
		}
		saveAllSettings();
	});

	ai3Button_ = createButton(activeAis_[2].getName(), hDistance_, [&](gui::Widget* item) {
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
			activeAis_[2] = ais_[(index + 1) % ais_.size()];
			button->setText(activeAis_[2].getName());
		}
		else {
			// Default ai.
			activeAis_[2] = Ai();
		}
		saveAllSettings();
	});

	ai4Button_ = createButton(activeAis_[3].getName(), hDistance_, [&](gui::Widget* item) {
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
			activeAis_[3] = ais_[(index + 1) % ais_.size()];
			button->setText(activeAis_[3].getName());
		}
		else {
			// Default ai.
			activeAis_[3] = Ai();
		}
		saveAllSettings();
	});

	gui::ButtonPtr reloadAis = createButton("Reload Ai:s", hDistance_, [&](gui::Widget*) {
		loadAllSettings();
		bool ai1 = false, ai2 = false, ai3 = false, ai4 = false;
		for (const Ai& ai : ais_) {
			if (ai.getName() == ai1Button_->getText()) {
				ai1 = true;
			}
			if (ai.getName() == ai2Button_->getText()) {
				ai2 = true;
			}
			if (ai.getName() == ai3Button_->getText()) {
				ai3 = true;
			}
			if (ai.getName() == ai4Button_->getText()) {
				ai4 = true;
			}
		}
		if (!ai1) {
			// Set to default ai.
			activeAis_[0] = Ai();
			ai1Button_->setText(activeAis_[0].getName());
		}
		if (!ai2) {
			// Set to default ai.
			activeAis_[1] = Ai();
			ai1Button_->setText(activeAis_[1].getName());
		}
		if (!ai3) {
			// Set to default ai.
			activeAis_[2] = Ai();
			ai1Button_->setText(activeAis_[2].getName());
		}
		if (!ai4) {
			// Set to default ai.
			activeAis_[3] = Ai();
			ai1Button_->setText(activeAis_[3].getName());
		}

	});
}

*/
