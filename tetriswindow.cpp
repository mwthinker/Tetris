#include "tetriswindow.h"

#include "gamesprite.h"
#include "gamefont.h"
#include "tetrisgame.h"
#include "joystick.h"
#include "keyboard.h"

#include "textbutton.h"
#include "highscore.h"

#include <mw/sprite.h>
#include <mw/color.h>

#include <iostream>
#include <fstream>
#include <sstream>

TetrisWindow::TetrisWindow() {
    numberOfLocalPlayers_ = 1;
	tetrisGame_.addCallback([&](NetworkEventPtr nEvent) {
		handleConnectionEvent(nEvent);
	});

	// Initializes default keybord devices for two players.
	DevicePtr device1(new Keyboard("Keyboard 1", SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP));
	devices_.push_back(device1);
	DevicePtr device2(new Keyboard("Keyboard 2", SDLK_s, SDLK_a, SDLK_d, SDLK_w));
	devices_.push_back(device2);

	// Init joysticks!
	const std::vector<mw::JoystickPtr>& joystics = mw::Joystick::getJoystics();
	for (const mw::JoystickPtr& joystick : joystics) {
		std::cout << joystick->getName() << std::endl;
		DevicePtr device(new Joystick(joystick, 0, 1));
		devices_.push_back(device);
	}

	loadHighscore();
}

TetrisWindow::~TetrisWindow() {
}

void TetrisWindow::setNumberOfLocalPlayers(int number) {
    const int size = devices_.size();
    if (number <= size) {
        numberOfLocalPlayers_ = number;
    } else {
        numberOfLocalPlayers_ = size;
    }
}

int TetrisWindow::getNumberOfLocalPlayers() const {
    return numberOfLocalPlayers_;
}

void TetrisWindow::abortGame() {
	tetrisGame_.closeGame();
}

void TetrisWindow::createLocalGame() {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < numberOfLocalPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.createLocalGame(tmpDevices);
	tetrisGame_.startGame();
}

void TetrisWindow::createLocalGame(int width, int height, int maxLevel) {
	const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < numberOfLocalPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.createLocalGame(tmpDevices, width, height, maxLevel);
	tetrisGame_.startGame();
}

void TetrisWindow::createServerGame(int port, int width, int height) {
    const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < numberOfLocalPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.createServerGame(tmpDevices,port, width, height);
}

void TetrisWindow::createClientGame(int port, std::string ip) {
    const int size = devices_.size();
	std::vector<DevicePtr> tmpDevices;
	for (int i = 0; i < numberOfLocalPlayers_ && i < size; ++i) {
		tmpDevices.push_back(devices_[i]);
	}

	tetrisGame_.closeGame();
	tetrisGame_.createClientGame(tmpDevices,port,ip);
}

void TetrisWindow::restartGame() {
	tetrisGame_.restartGame();
}

void TetrisWindow::startGame() {
	tetrisGame_.startGame();
}

bool TetrisWindow::isPaused() const {
	return tetrisGame_.isPaused();
}

void TetrisWindow::changePauseState() {
	tetrisGame_.pause();
}

bool TetrisWindow::isReady() const {
	return tetrisGame_.isReady();
}

void TetrisWindow::changeReadyState() {
	tetrisGame_.changeReadyState();
}

void TetrisWindow::updateGame(Uint32 deltaTime) {
	tetrisGame_.update(deltaTime);
}

void TetrisWindow::drawGame(Uint32 deltaTime) {
	glPushMatrix();
	int w = getWidth();
	int h = getHeight() - 30;

	int x, y;
	int width, height;

	// Centers the game and holds the correct proportions. The sides is transparent.
	if (tetrisGame_.getWidth() / w > tetrisGame_.getHeight() / h) {
		// Black sides, up and down.
		double scale = w / tetrisGame_.getWidth();
		glTranslated(0, (h - scale*tetrisGame_.getHeight()) * 0.5, 0);
		glScaled(scale, scale, 1);
		x = 0;
		y = static_cast<int>((h - scale*tetrisGame_.getHeight()) * 0.5);
		width = w;
		height = static_cast<int>(scale*tetrisGame_.getHeight());
	} else {
		// Black sides, left and right.
		double scale = h / tetrisGame_.getHeight();
		glTranslated((w-scale*tetrisGame_.getWidth()) * 0.5, 0, 0);
		glScaled(scale,scale,1);
		x = static_cast<int>((w-scale*tetrisGame_.getWidth()) * 0.5);
		y = 0;
		width = static_cast<int>(scale*tetrisGame_.getWidth());
		height = h;
	}

	glEnable(GL_SCISSOR_TEST);
	glScissor(x,y,width,height);
	tetrisGame_.draw();
	glPopMatrix();

	glDisable(GL_SCISSOR_TEST);
}

void TetrisWindow::updateGameEvent(const SDL_Event& windowEvent) {
	for (DevicePtr& device : devices_) {
		device->eventUpdate(windowEvent);
	}
}

void TetrisWindow::handleConnectionEvent(NetworkEventPtr nEvent) {
	if (std::shared_ptr<GameOver> gameOver = std::dynamic_pointer_cast<GameOver>(nEvent)) {
		HighscorePtr highscore = getHighscorePtr();
		// Points high enough to be saved in the highscore list?
		if (highscore->isNewRecord(gameOver->points_)) {
			// Set points in order for highscore to know which point to save in list.
			highscore->setNextRecord(gameOver->points_);
			// In order for the user to insert name.
			highscore->click();
		}
	} else if (std::shared_ptr<GamePause> gameOver = std::dynamic_pointer_cast<GamePause>(nEvent)) {
		if (gameOver->pause_) {
			getPausePtr()->setText("Unpause");
		} else {
			getPausePtr()->setText("Pause");
		}
	} else if (std::shared_ptr<NewConnection> newConnection = std::dynamic_pointer_cast<NewConnection>(nEvent)) {
		auto networkLooby = getNetworkLoobyPtr();
		networkLooby->clear();
		newConnection->iterate([&](int id, int nbrOfPlayers, bool ready) {
			networkLooby->addConnection(id,nbrOfPlayers, ready);
		});

		switch (newConnection->status_) {
		case NewConnection::CLIENT:
			gotoClientLoobyFrame();
			break;
		case NewConnection::SERVER:
			gotoServerLoobyFrame();
			break;
		case NewConnection::LOCAL:
			// Is no local looby.
			break;
		}
	} else if (std::shared_ptr<GameStart> start = std::dynamic_pointer_cast<GameStart>(nEvent)) {
		switch (start->status_) {
		case GameStart::LOCAL:
			gotoLocalPlayFrame();
			break;
		case GameStart::CLIENT:
			gotoClientPlayFrame();
			break;
		case GameStart::SERVER:
			gotoServerPlayFrame();
			break;
		}
	} else if (std::shared_ptr<GameReady> ready = std::dynamic_pointer_cast<GameReady>(nEvent)) {
		auto networkLooby = getNetworkLoobyPtr();
		networkLooby->setReady(ready->id_,ready->ready_);
	}
}

void TetrisWindow::loadHighscore() {
	std::string line;
	std::ifstream file("highscore");
	if (file.is_open()) {
		HighscorePtr highscore = getHighscorePtr();
		while (file.good()) {
			std::getline(file, line);
			if (line.size() < 6) {
				break;
			}

			// Line should look like.
			// line = "'Marcus' 2013-05-19 2340".
			unsigned pos1 = line.find('\'');
			unsigned pos2 = line.find('\'', pos1+1);
			std::string name = line.substr(pos1+1, pos2-pos1-1); // name = "Marcus".

			line = line.substr(pos2+1); // line = " 2013-05-19 2340".
			std::stringstream strStream(line);
			std::string date;
			int record;
			strStream >> date >> record;

			highscore->setNextRecord(record);
			highscore->addNewRecord(name, date);
		}
		file.close();
	}
}

void TetrisWindow::saveHighscore() {
	std::string line;
	std::ofstream file("highscore");
	HighscorePtr highscore = getHighscorePtr();
	if (file.is_open() && file.good()) {
		// Line should look like.
		// line = "'Marcus' 2013-05-19 2340".
		highscore->iterateRecords([&](int points,std::string name, std::string date) {
			file << "'" << name << "' " << date << " " << points << "\n";
		});
	}
	file.close();
}
