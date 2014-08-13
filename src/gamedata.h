#ifndef GAMEDATA_H
#define GAMEDATA_H

#include "ai.h"

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/color.h>

#include <tinyxml2.h>

#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>

class GameData {
public:
	class Highscore;

	GameData(std::string dataFile);	

	void save();
	
	inline mw::Font getDefaultFont(int size) {
		return loadFont(font_, size);
	}

	bool getWindowBorder() const;
	int getWindowWidth() const;
	int getWindowHeight() const;
	void setWindowSize(int width, int height);
	void setWindowPosition(int x, int y);
	int getWindowXPosition() const;
	int getWindowYPosition() const;

	void setWindowMaximized(bool maximized);
	bool isWindowMaximized() const;

	std::vector<Ai> getAis() const;

	std::vector<Highscore> getHighscoreVector() const;
	void setHighscoreVector(const std::vector<Highscore>& v);

	std::string getIconPath() const;
	
	// Bar.
	mw::Color barColor_;
	float barHeight_;

	// Label.
	mw::Color labelTextColor_;
	mw::Color labelBackgroundColor_;

	// Button.
	mw::Color buttonFocusColor_;
	mw::Color buttonTextColor_;
	mw::Color buttonHoverColor_;
	mw::Color buttonPushColor_;
	mw::Color buttonBackgroundColor_;
	mw::Color buttonBorderColor_;

	// Sounds.
	mw::Sound soundButtonPush_;
	mw::Sound soundBlockCollision_;
	mw::Sound soundRowRemoved_;
	mw::Sound soundTetris_;
	mw::Sound soundHighscore_;

	// Game.
	int maxNbrAis_;
	int maxNbrHumans_;

	// Sprites.
	mw::Sprite spriteBackground_;
	mw::Sprite spriteComputer_;
	mw::Sprite spriteHuman_;
	mw::Sprite spriteCross_;
	mw::Sprite spriteZoom_;
	mw::Sprite spriteZ_, spriteS_, spriteJ_, spriteI_, spriteL_, spriteT_, spriteO_;

	class Highscore {
	public:
		Highscore() {
		}

		Highscore(std::string name, int points, std::string date) : name_(name), points_(points), date_(date) {
		}

		std::string date_;
		int points_;
		std::string name_;
	};
	
private:
	void load(tinyxml2::XMLConstHandle handle);
	void loadWindow(tinyxml2::XMLConstHandle handle);
	void loadGame(tinyxml2::XMLConstHandle handle) const;
	void loadNetwork(tinyxml2::XMLConstHandle handle) const;
	void loadPlayers(tinyxml2::XMLConstHandle handle) const;

	mw::Sprite extractSprite(tinyxml2::XMLConstHandle handle) const;
	mw::Sound extractSound(tinyxml2::XMLConstHandle handle) const;

	// Font path.
	std::string font_;

	// Icon path.
	std::string icon_;

	mw::Font loadFont(std::string file, unsigned int fontSize) const;
	mw::Texture loadTexture(std::string file) const;
	mw::Sound loadSound(std::string file) const;
	
	mutable std::map<std::string, mw::Font> fonts_;
	mutable std::map<std::string, mw::Texture> textures_;
	mutable std::map<std::string, mw::Sound> sounds_;

	tinyxml2::XMLDocument xmlDoc_;
	std::string dataFile_;
};

#endif // ZOMBIEENGINE_H
