#ifndef TETRISWINDOW_H
#define TETRISWINDOW_H

#include "sdldevice.h"
#include "ai.h"
#include "tetrisgame.h"

#include <gui/frame.h>
#include <gui/textfield.h>
#include <gui/button.h>
#include <gui/panel.h>
#include <gui/label.h>
#include <gui/checkbox.h>
#include <gui/progressbar.h>

#include <array>
#include <memory>

class ManButton;
class Highscore;
class GameComponent;

class TetrisGameEvent;
class GameData;

class TetrisWindow : public gui::Frame {
public:
	TetrisWindow();

	~TetrisWindow();

	void startServerLoop(int port);

	void startClientLoop(int port, std::string ip);

private:
	enum class StartFrame { MENU, SERVER, CLIENT, LOCAL_GAME };

	void resumeGame();

	void saveCurrentLocalGame();

	void initOpenGl() override;

	void initPreLoop() override;

	void updateDevices(gui::Frame& frame, const SDL_Event& windowEvent);

	void handleConnectionEvent(TetrisGameEvent& tetrisEvent);

	void loadHighscore();
	void saveHighscore();

	void setPlayers();

	DevicePtr findHumanDevice(std::string name) const;
	DevicePtr findAiDevice(std::string name) const;

	TetrisGame tetrisGame_;

	// initPlayPanel
	std::shared_ptr<GameComponent> game_;
	std::shared_ptr<ManButton> nbrHumans_;
	std::shared_ptr<ManButton> nbrAis_;
	std::shared_ptr<gui::Panel> manBar_;
	std::vector<std::shared_ptr<ManButton>> remoteManButtons;

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

	// initNetworkPanel
	std::shared_ptr<gui::Button> networkConnect_;
	std::shared_ptr<gui::CheckBox> radioButtonServer_;
	std::shared_ptr<gui::CheckBox> radioButtonClient_;
	std::shared_ptr<gui::TextField> ipClient_;
	std::shared_ptr<gui::TextField> port_;
	std::shared_ptr<gui::Label> clientIpLabel_;
	std::shared_ptr<gui::Label> errorMessage_;
	std::shared_ptr<gui::ProgressBar> progressBar_;

	// Devices.
	std::vector<SdlDevicePtr> devices_;
	int nbrOfHumanPlayers_, nbrOfComputerPlayers_;
	std::array<DevicePtr, 4> activeAis_;

	// All panels.
	void initMenuPanel();
	void initPlayPanel();
	void initHighscorePanel();
	void initNewHighscorePanel();
	void initCustomPlayPanel();
	void initSettingsPanel();
	void initNetworkPanel();

	void sdlEventListener(gui::Frame& frame, const SDL_Event& e);

	// Panel indexes.
	int menuIndex_,
		playIndex_,
		highscoreIndex_,
		customIndex_,
		settingsIndex_,
		newHighscoreIndex_,
		networkIndex_;

	bool windowFollowMouse_;
	int followMouseX_, followMouseY_;

	int lastX_, lastY_;
	StartFrame startFrame_;
};

#endif // TETRISWINDOW_H
