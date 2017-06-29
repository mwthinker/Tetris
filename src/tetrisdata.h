#ifndef TETRISDATA_H
#define TETRISDATA_H

#include "block.h"
#include "color.h"
#include "ai.h"
#include "tetrisgame.h"

#include <mw/sound.h>
#include <mw/sprite.h>
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
	inline static TetrisData& getInstance() {
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
	
	mw::Sound getBlockCollisionSound();
	mw::Sound getRowRemovedSound();
	mw::Sound getTetrisSound();
	mw::Sound getButtonPushSound();

	mw::Font getDefaultFont(int size);

	Color getOuterSquareColor();
	Color getInnerSquareColor();
	Color getStartAreaColor();
	Color getPlayerAreaColor();
	Color getBorderColor();

	mw::Sprite getBorderHorizontalSprite();
	mw::Sprite getBorderVerticalSprite();
	mw::Sprite getBorderLeftUpSprite();
	mw::Sprite getBorderRightUpSprite();
	mw::Sprite getBorderDownLeftSprite();
	mw::Sprite getBorderDownRightSprite();

	float getTetrisSquareSize();
	float getTetrisBorderSize();

	void bindTextureFromAtlas() const;

	mw::Sprite getHalfCircleSprite();
	mw::Sprite getLineSegmentSprite();

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

	bool isFullscreenOnDoubleClick();
	void setFullscreenOnDoubleClick(bool activate);

	bool isMoveWindowByHoldingDownMouse();
	void setMoveWindowByHoldingDownMouse(bool activate);
	
	mw::Sprite getBackgroundSprite();

	std::vector<Ai> getAiVector();
	std::vector<HighscoreRecord> getHighscoreRecordVector();
	void setHighscoreRecordVector(const std::vector<HighscoreRecord>& highscoreVector);

	float getWindowBarHeight();

	Color getWindowBarColor();

	mw::Sprite getCheckboxBoxSprite();
	mw::Sprite getCheckboxCheckSprite();
	Color getCheckboxTextColor();
	Color getCheckboxBackgroundColor();
	Color getCheckboxBoxColor();
	Color getChecboxCheckColor();

	Color getLabelTextColor();
	Color getLabelBackgroundColor();

	Color getButtonFocusColor();
	Color getButtonTextColor();
	Color getButtonHoverColor();
	Color getButtonPushColor();
	Color getButtonBackgroundColor();
	Color getButtonBorderColor();

	mw::Sprite getHumanSprite();
	mw::Sprite getComputerSprite();
	mw::Sprite getCrossSprite();
	mw::Sprite getZoomSprite();
	
	void setActiveLocalGame(int rows, int columns, std::vector<PlayerData> playerDataVector);
	std::vector<PlayerData> getActiveLocalGamePlayers();
	int getActiveLocalGameRows();
	int getActiveLocalGameColumns();
	
private:
	TetrisData();
	
	const std::string JSON_PATH = "tetris.json";
	mw::TextureAtlas textureAtlas_;
	std::map<std::string, mw::Sound> sounds_;
	std::map<std::string, mw::Font> fonts_;
	std::map<std::string, mw::Music> musics_;
	nlohmann::json jsonObject_;
};

#endif // TETRISDATA_H
