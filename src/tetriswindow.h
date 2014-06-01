#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "device.h"
#include "ai.h"
#include "tetrisgame.h"

#include <gui/frame.h>
#include <gui/textfield.h>
#include <gui/button.h>

#include <array>
#include <memory>

class ManButton;
class Highscore;
class NetworkLooby;
class GameComponent;

class NetworkEvent;
typedef std::shared_ptr<NetworkEvent> NetworkEventPtr;

class GameData;

class TetrisWindow : public gui::Frame {
public:
	TetrisWindow(GameData& gameData);
	~TetrisWindow();

private:
	void updateDevices(Frame& frame, const SDL_Event& windowEvent);
	
	void createLocalGame(int width, int height, int maxLevel);
	void createServerGame(int port, int width, int height);
	void createClientGame(int port, std::string ip);

	void handleConnectionEvent(NetworkEventPtr nEvent);

	void loadHighscore();
	void saveHighscore();
	
	void loadAllSettings();
	void saveAllSettings();

	TetrisGame tetrisGame_;

	// initMenuPanel
	std::shared_ptr<gui::Button> resume_;

	// initPlayPanel
	std::shared_ptr<GameComponent> game_;
	std::shared_ptr<ManButton> nbrHumans_;
	std::shared_ptr<ManButton> nbrAis_;
	std::shared_ptr<gui::Button> pauseButton_;
	std::shared_ptr<gui::Button> menu_;
	std::shared_ptr<gui::Button> restart_;
	
	// initHighscorePanel
	std::shared_ptr<Highscore> highscore_;
	
	// initNewHighscorePanel
	std::shared_ptr<gui::TextField> textField_;

	// initCustomPlayPanel
	std::shared_ptr<gui::TextField> customWidthField_;
	std::shared_ptr<gui::TextField> customHeightField_;
	std::shared_ptr<gui::TextField> customMinLevel_;
	std::shared_ptr<gui::TextField> customMaxLevel_;
	
	// initCreateServerPanel
	std::shared_ptr<gui::TextField> portServer_;
	std::array<std::shared_ptr<gui::Panel>, 4> playersServer_;
	std::array<std::shared_ptr<gui::TextField>, 4> namesServer_;
	std::shared_ptr<ManButton> nbrHumansServer_;
	std::shared_ptr<ManButton> nbrAisServer_;
	std::shared_ptr<gui::TextField> serverWidthField_;
	std::shared_ptr<gui::TextField> serverHeightField_;
	std::shared_ptr<gui::TextField> serverMinLevel_;
	std::shared_ptr<gui::TextField> serverMaxLevel_;

	// initCreateClientPanel
	std::shared_ptr<gui::TextField> ipClient_;
	std::shared_ptr<gui::TextField> portClient_;
	std::array<std::shared_ptr<gui::Panel>, 4> playersClient_;
	std::array<std::shared_ptr<gui::TextField>, 4> namesClient_;
	std::shared_ptr<ManButton> nbrHumansClient_;
	std::shared_ptr<ManButton> nbrAisClient_;

	// initServerLoobyPanel
	std::shared_ptr<NetworkLooby> serverLooby_;
	std::shared_ptr<gui::Button> serverReady_;;
	
	// initServerLoobyPanel
	std::shared_ptr<NetworkLooby> clientLooby_;
	std::shared_ptr<gui::Button> clientReady_;;

	// All ai:s.
	std::array<Ai, 4> activeAis_;
	std::vector<Ai> ais_;

	// Devices.
	std::vector<DevicePtr> devices_;
	int nbrOfHumanPlayers_, nbrOfComputerPlayers_;

	// All panels.
	void initMenuPanel();
	void initPlayPanel();
	void initHighscorePanel();
	void initNewHighscorePanel();
	void initCustomPlayPanel();
	void initSettingsPanel();
	void initCreateServerPanel();
	void initCreateClientPanel();
	void initServerLoobyPanel();
	void initClientLoobyPanel();
	void initWaitToConnectPanel();
	void initNewHighScorePanel();

	// Panel indexes.
	int menuIndex_,
		playIndex_,
		networkIndex_,
		highscoreIndex_,
		customIndex_,
		settingsIndex_,
		newHighscoreIndex_,
		createClientIndex_,
		createServerIndex_,
		loobyClientIndex_,
		loobyServerIndex_,
		waitToConnectIndex_,
		networkPlayIndex_,
		aiIndex_;

	GameData& gameData_;
};

#endif // TETRISWINDOW_H
