#ifndef TETRISDATA_H
#define TETRISDATA_H

#include "block.h"
#include "ai.h"
#include "tetrisgame.h"

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/color.h>
#include <mw/font.h>
#include <mw/music.h>
#include <mw/textureatlas.h>

#include <json.hpp>

#include <map>
#include <vector>

class HighscoreRecord {
public:
	HighscoreRecord() : points_(0), level_(1), rows_(0) {
	}

	HighscoreRecord(std::string name, std::string date, int points, int level, int rows)
		: name_(name), date_(date), points_(points), level_(level), rows_(rows) {
	}

	std::string name_;
	std::string date_;
	int points_;
	int level_;
	int rows_;
};

class TetrisData {
public:
	static TetrisData& getInstance() {
		static TetrisData instance;
		return instance;
	}

	TetrisData(TetrisData const&) = delete;
	TetrisData& operator=(const TetrisData&) = delete;

	void save();

	mw::Font loadFont(std::string file, unsigned int fontSize);
	mw::Sound loadSound(std::string file);
	mw::Music loadMusic(std::string file);
	mw::Sprite loadSprite(std::string file);

	mw::Sprite getSprite(BlockType blockType);

	mw::Font getDefaultFont(int size);

	mw::Color getOuterSquareColor() const;
	mw::Color getInnerSquareColor() const;
	mw::Color getStartAreaColor() const;
	mw::Color getPlayerAreaColor() const;
	mw::Color getBorderColor() const;

	float getTetrisSquareSize() const;
	float getTetrisBorderSize() const;

	void bindTextureFromAtlas() const;

	bool isLimitFps() const;
	void setLimitFps(bool limited);

	int getWindowPositionX() const;
	int getWindowPositionY() const;

	void setWindowPositionX(int x);
	void setWindowPositionY(int y);

	int getWindowWidth() const;
	int getWindowHeight() const;

	void setWindowWidth(int width);
	void setWindowHeight(int height);

	bool isWindowResizable() const;
	void setWindowResizable(bool resizeable);

	int getWindowMinWidth() const;
	int getWindowMinHeight() const;
	std::string getWindowIcon() const;

	bool isWindowPauseOnLostFocus() const;
	void setWindowPauseOnLostFocus(bool pauseOnFocus);

	bool isWindowBordered() const;
	void setWindowBordered(bool border);

	bool isWindowMaximized() const;
	void setWindowMaximized(bool maximized);

	bool isWindowVsync() const;
	void setWindowVsync(bool activate);

	int getMultiSampleBuffers() const;
	int getMultiSampleSamples() const;

	float getRowFadingTime() const;
	void setRowFadingTime(float time);

	float getRowMovingTime() const;
	void setRowMovingTime(float time);

	bool isFullscreenOnDoubleClick() const;
	void setFullscreenOnDoubleClick(bool activate);

	bool isMoveWindowByHoldingDownMouse() const;
	void setMoveWindowByHoldingDownMouse(bool activate);

	int getPort() const;
	void setPort(int port);

	int getTimeToConnectMS() const;

	std::string getIp() const;
	void setIp(std::string ip);

	mw::Sprite getBackgroundSprite();

	std::string getAi1Name() const;
	std::string getAi2Name() const;
	std::string getAi3Name() const;
	std::string getAi4Name() const;

	void setAi1Name(std::string name);
	void setAi2Name(std::string name);
	void setAi3Name(std::string name);
	void setAi4Name(std::string name);

	std::vector<Ai> getAiVector() const;

	std::vector<HighscoreRecord> getHighscoreRecordVector() const;
	void setHighscoreRecordVector(const std::vector<HighscoreRecord>& highscoreVector);

	float getWindowBarHeight() const;

	mw::Color getWindowBarColor() const;

	mw::Sprite getCheckboxBoxSprite();
	mw::Sprite getCheckboxCheckSprite();
	mw::Color getCheckboxTextColor() const;
	mw::Color getCheckboxBackgroundColor() const;
	mw::Color getCheckboxBoxColor() const;
	mw::Color getChecboxCheckColor() const;

	mw::Sprite getRadioButtonBoxSprite();
	mw::Sprite getRadioButtonCheckSprite();
	mw::Color getRadioButtonTextColor() const;
	mw::Color getRadioButtonBackgroundColor() const;
	mw::Color getRadioButtonBoxColor() const;
	mw::Color getRadioButtonCheckColor() const;

	mw::Color getLabelTextColor() const;
	mw::Color getLabelBackgroundColor() const;

	mw::Color getButtonFocusColor() const;
	mw::Color getButtonTextColor() const;
	mw::Color getButtonHoverColor() const;
	mw::Color getButtonPushColor() const;
	mw::Color getButtonBackgroundColor() const;
	mw::Color getButtonBorderColor() const;

	mw::Color getComboBoxFocusColor() const;
	mw::Color getComboBoxTextColor() const;
	mw::Color getComboBoxSelectedTextColor() const;
	mw::Color getComboBoxSelectedBackgroundColor() const;
	mw::Color getComboBoxBackgroundColor() const;
	mw::Color getComboBoxBorderColor() const;
	mw::Color getComboBoxShowDropDownColor() const;
	mw::Sprite getComboBoxShowDropDownSprite();

	mw::Sprite getHumanSprite();
	mw::Sprite getComputerSprite();
	mw::Sprite getCrossSprite();
	mw::Sprite getZoomSprite();

	void setActiveLocalGame(int rows, int columns, const std::vector<PlayerData>& playerDataVector);
	std::vector<PlayerData> getActiveLocalGamePlayers();
	int getActiveLocalGameRows() const;
	int getActiveLocalGameColumns() const;

private:
	TetrisData();

	std::string jsonPath_;
	mw::TextureAtlas textureAtlas_;
	std::map<std::string, mw::Sound> sounds_;
	std::map<std::string, mw::Font> fonts_;
	std::map<std::string, mw::Music> musics_;
	nlohmann::json jsonObject_;
};

#endif // TETRISDATA_H
