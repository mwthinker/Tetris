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
	HighscoreRecord() : points_(0) {
	}

	HighscoreRecord(std::string name, std::string date, int points) : name_(name), date_(date), points_(points) {
	}

	std::string name_;
	std::string date_;
	int points_;
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

	mw::Color getOuterSquareColor();
	mw::Color getInnerSquareColor();
	mw::Color getStartAreaColor();
	mw::Color getPlayerAreaColor();
	mw::Color getBorderColor();

	float getTetrisSquareSize();
	float getTetrisBorderSize();

	void bindTextureFromAtlas() const;

	int getWindowPositionX();
	int getWindowPositionY();

	void setWindowPositionX(int x);
	void setWindowPositionY(int y);

	int getWindowWidth();
	int getWindowHeight();

	void setWindowWidth(int width);
	void setWindowHeight(int height);

	bool isWindowResizable();
	void setWindowResizable(bool resizeable);

	int getWindowMinWidth();
	int getWindowMinHeight();
	std::string getWindowIcon();
	bool isWindowBordered();
	void setWindowBordered(bool border);
	
	bool isWindowMaximized();
	void setWindowMaximized(bool maximized);

	bool isWindowVsync();
	void setWindowVsync(bool activate);

	int getMultiSampleBuffers() const;
	int getMultiSampleSamples() const;

	float getRowFadingTime() const;
	void setRowFadingTime(float time);

	float getRowMovingTime() const;
	void setRowMovingTime(float time);

	bool isFullscreenOnDoubleClick();
	void setFullscreenOnDoubleClick(bool activate);

	bool isMoveWindowByHoldingDownMouse();
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

	std::vector<Ai> getAiVector();
	
	std::vector<HighscoreRecord> getHighscoreRecordVector();
	void setHighscoreRecordVector(const std::vector<HighscoreRecord>& highscoreVector);

	float getWindowBarHeight();

	mw::Color getWindowBarColor();

	mw::Sprite getCheckboxBoxSprite();
	mw::Sprite getCheckboxCheckSprite();
	mw::Color getCheckboxTextColor();
	mw::Color getCheckboxBackgroundColor();
	mw::Color getCheckboxBoxColor();
	mw::Color getChecboxCheckColor();

	mw::Sprite getRadioButtonBoxSprite();
	mw::Sprite getRadioButtonCheckSprite();
	mw::Color getRadioButtonTextColor();
	mw::Color getRadioButtonBackgroundColor();
	mw::Color getRadioButtonBoxColor();
	mw::Color getRadioButtonCheckColor();

	mw::Color getLabelTextColor();
	mw::Color getLabelBackgroundColor();

	mw::Color getButtonFocusColor();
	mw::Color getButtonTextColor();
	mw::Color getButtonHoverColor();
	mw::Color getButtonPushColor();
	mw::Color getButtonBackgroundColor();
	mw::Color getButtonBorderColor();

	mw::Color getComboBoxFocusColor();
	mw::Color getComboBoxTextColor();
	mw::Color getComboBoxSelectedTextColor();
	mw::Color getComboBoxSelectedBackgroundColor();
	mw::Color getComboBoxBackgroundColor();
	mw::Color getComboBoxBorderColor();
	mw::Color getComboBoxShowDropDownColor();
	mw::Sprite getComboBoxShowDropDownSprite();

	mw::Sprite getHumanSprite();
	mw::Sprite getComputerSprite();
	mw::Sprite getCrossSprite();
	mw::Sprite getZoomSprite();
	
	void setActiveLocalGame(int rows, int columns, const std::vector<PlayerData>& playerDataVector);
	std::vector<PlayerData> getActiveLocalGamePlayers();
	int getActiveLocalGameRows();
	int getActiveLocalGameColumns();
	
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
