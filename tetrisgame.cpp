#include "tetrisgame.h"

#include "gamesound.h"
#include "graphicboard.h"
#include "player.h"
#include "human.h"
#include "inputdevice.h"
#include "inputkeyboard.h"
#include "inputjoystick.h"

#include <mw/sound.h>

#include <iostream>
#include <utility>
#include <memory>

TetrisGame::TetrisGame() {
	// Initializes default keybord devices for two players.
	InputDevicePtr inputDevice1(new InputKeyboard(SDLK_DOWN, SDLK_LEFT, SDLK_RIGHT, SDLK_UP));
	inputDevices_.push_back(inputDevice1);
	InputDevicePtr inputDevice2(new InputKeyboard(SDLK_s, SDLK_a, SDLK_d, SDLK_w));
	inputDevices_.push_back(inputDevice2);

	// Init joysticks!
	auto joystics = mw::Joystick::getJoystics();
	for(mw::JoystickPtr& joystick : joystics) {
		std::cout << joystick->getName() << std::endl;
		InputDevicePtr inputDevice(new InputJoystick(joystick,0,1));
		inputDevices_.push_back(inputDevice);
	}
}

void TetrisGame::createNewHumanPlayers(int nbrOfLocalPlayers) {
	if (manager_.getStatus() == PlayerManager::WAITING_TO_CONNECT) {
		humanPlayers_.clear();	
		for (int i = 0; i < nbrOfLocalPlayers; ++i) {
			HumanPtr human(new Human());
			humanPlayers_.push_back(human);
		}
		std::cout << "\n" << "createNewHumanPlayers";
	}
}

void TetrisGame::createLocalGame(int nbrOfLocalPlayers) {
	createNewHumanPlayers(nbrOfLocalPlayers);
	manager_.connect(humanPlayers_,PlayerManager::LOCAL);
}

void TetrisGame::createServerGame(int nbrOfLocalPlayers, int port) {
	createNewHumanPlayers(nbrOfLocalPlayers);
	//manager_.setPort();
	manager_.setServerPort(port);
	manager_.connect(humanPlayers_,PlayerManager::SERVER);
}

void TetrisGame::createClientGame(int nbrOfLocalPlayers,int port, std::string ip) {	
	if (manager_.getStatus() == PlayerManager::WAITING_TO_CONNECT) {
		createNewHumanPlayers(nbrOfLocalPlayers);
		manager_.setConnectToIp(ip);
		manager_.setConnectToPort(port);
		manager_.connect(humanPlayers_,PlayerManager::CLIENT);
	}
}

void TetrisGame::startGame() {
	manager_.start();
}

void TetrisGame::restartGame() {
	manager_.restart();
}

void TetrisGame::closeGame() {
	manager_.stop();
}

void TetrisGame::setReadyGame(bool ready) {
	manager_.setReady(ready);
}

/*
bool TetrisGame::updateAddNewHumans(int nbrOfLocalPlayers) {
	std::vector<HumanPtr> tmp;
	for (int i = 0; i < nbrOfLocalPlayers; ++i) {
		HumanPtr human(new Human());
		tmp.push_back(human);
	}
	if (manager_.updateAddNewHumans(tmp)) {
		humanPlayers_.insert(humanPlayers_.begin(),tmp.begin(), tmp.end());
		return true;
	}
	return false;
}
*/

void TetrisGame::update(Uint32 deltaTime) {
	manager_.update(deltaTime);

	if (manager_.isStarted()) {
		auto& players = manager_.getPlayers();
		// Iterate through all players and progress the game and apply game rules and sound effects.
		for (Player* player : players) {
			GameEvent gameEvent;
			while (player->pollGameEvent(gameEvent)) {
				manager_.applyRules(player, gameEvent);
				soundEffects(gameEvent);
			}
		}
	}	
}

void TetrisGame::draw() {
	glPushMatrix();	
	
	auto& players = manager_.getPlayers();
	for (auto it = players.begin(); it != players.end(); ++it) {
		Player* player = *it;
		player->draw();
		glTranslated(player->getWidth(),0,0);
	}
	
	glPopMatrix();
}

void TetrisGame::eventUpdate(const SDL_Event& windowEvent) {
	// Takes care of user input for all human players.
	for (auto it = humanPlayers_.begin(); it != humanPlayers_.end(); ++it) {
		HumanPtr& human = *it;
		unsigned int index = it - humanPlayers_.begin();
		if (index < inputDevices_.size()) {
			InputDevicePtr& device = inputDevices_[index];
			device->eventUpdate(windowEvent);

			PlayerEvent playerEvent;
			while (device->pollEvent(playerEvent)) {
				human->updatePlayerEvent(playerEvent);
			}
		}
	}
}

double TetrisGame::getWidth() const {
	double width = 0;
	const auto& players = manager_.getPlayers();
	
	for (Player* player : players) {
		width += player->getWidth();
	}
	return width;
}

double TetrisGame::getHeight() const {
	auto& players = manager_.getPlayers();
	if (players.size() == 0) {
		return 400;
	}
	return players.front()->getHeight();
}

void TetrisGame::soundEffects(GameEvent gameEvent) {
	mw::Sound sound;
	switch (gameEvent) {
	case GRAVITY_MOVES_BLOCK:
		break;
	case BLOCK_COLLISION:
		sound = soundBlockCollision;
		break;
	case PLAYER_ROTATES_BLOCK:
		break;
	case ONE_ROW_REMOVED:
		// Fall through
	case TWO_ROW_REMOVED:
		// Fall through
	case THREE_ROW_REMOVED:
		sound = soundRowRemoved;
		break;
	case FOUR_ROW_REMOVED:
		sound = soundTetris;
		break;
	case GAME_OVER:
		break;
	default:
		break;
	}
	sound.play();
}
