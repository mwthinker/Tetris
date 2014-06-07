#ifndef GAMEDATA_H
#define GAMEDATA_H

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/color.h>

#include <tinyxml2.h>

#include <map>
#include <string>

class GameData {
public:
	GameData(std::string dataFile);

	void save();
	
	inline mw::Font getDefaultFont(int size) {
		return loadFont(font_, size);
	}

	int getWindowWidth() const;
	int getWindowHeight() const;
	void setWindowSize(int width, int height);

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
	mw::Sound soundBlockCollision;
	mw::Sound soundRowRemoved;
	mw::Sound soundTetris;
	mw::Sound soundHighscore;

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
	
private:
	void load(tinyxml2::XMLHandle handle);
	void loadWindow(tinyxml2::XMLHandle handle);
	void loadGame(tinyxml2::XMLHandle handle);
	void loadNetwork(tinyxml2::XMLHandle handle);
	void loadPlayers(tinyxml2::XMLHandle handle);
	void loadHighscore(tinyxml2::XMLHandle handle);

	void extract(mw::Sprite& sprite, tinyxml2::XMLHandle handle);
	void extract(mw::Sound& sound, tinyxml2::XMLHandle handle);

	// Font path.
	std::string font_;

	// Icon path.
	std::string icon_;

	mw::Font loadFont(std::string file, unsigned int fontSize);
	mw::Texture loadTexture(std::string file);
	mw::Sound loadSound(std::string file);
	
	std::map<std::string, mw::Font> fonts_;
	std::map<std::string, mw::Texture> textures_;
	std::map<std::string, mw::Sound> sounds_;

	tinyxml2::XMLDocument xmlDoc_;
	std::string dataFile_;
};

#endif // ZOMBIEENGINE_H
