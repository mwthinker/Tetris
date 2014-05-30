#include "gamedata.h"

#include <mw/exception.h>

#include <tinyxml2.h>

#include <sstream>

namespace {

	std::stringstream& operator>>(std::stringstream& stream, mw::Color& color) {
		return stream;
	}

	template <class Output>
	Output extract(tinyxml2::XMLHandle handle) {
		tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw mw::Exception("Missing element!");
		}
		const char* str = element->GetText();

		if (str == nullptr) {
			throw mw::Exception("Missing text!");
		}

		std::stringstream stream(str);
		Output output;
		stream >> output;
		return output;
	}	

	template <class Output>
	void extract(Output& value, tinyxml2::XMLHandle handle) {
		value = extract<Output>(handle);
	}

}

GameData::GameData(std::string dataFile) {
	// Load XML file.
	tinyxml2::XMLDocument xmlDoc;
	xmlDoc.LoadFile(dataFile.c_str());
	if (xmlDoc.Error()) {
		// Failed!
		xmlDoc.PrintError();
	}
	
	tinyxml2::XMLHandle handleXml(xmlDoc.FirstChildElement("tetris"));

	// Load all data.
	load(handleXml);
}

void GameData::load(tinyxml2::XMLHandle handle) {
	loadWindow(handle.FirstChildElement("window"));
	loadGame(handle.FirstChildElement("game"));
	loadNetwork(handle.FirstChildElement("network"));
	loadPlayers(handle.FirstChildElement("players"));
	loadHighscore(handle.FirstChildElement("highscore"));
}

void GameData::loadWindow(tinyxml2::XMLHandle handle) {
	// <bar>.
	::extract(barHeight_, handle.NextSiblingElement("bar").FirstChildElement("height"));
	::extract(barColor_, handle.NextSiblingElement().FirstChildElement("color"));
	
	// <label>.
	::extract(labelTextColor_, handle.NextSiblingElement("label").FirstChildElement("textColor"));
	::extract(labelBackgroundColor_, handle.NextSiblingElement().FirstChildElement("backgroundColor"));
	
	// <button>.
	::extract(buttonFocusColor_, handle.NextSiblingElement("button").FirstChildElement("focusColor"));
	::extract(buttonTextColor_, handle.NextSiblingElement().FirstChildElement("textColor"));
	::extract(buttonHoverColor_, handle.NextSiblingElement().FirstChildElement("hoverColor"));
	::extract(buttonPushColor_, handle.NextSiblingElement().FirstChildElement("pushColor"));
	::extract(buttonBackgroundColor_, handle.NextSiblingElement().FirstChildElement("backgroundColor"));
	::extract(buttonBorderColor_, handle.NextSiblingElement().FirstChildElement("borderColor"));

	// <font>.
	::extract(font_, handle.NextSiblingElement("font"));

	// <width>.
	::extract(width_, handle.NextSiblingElement("width"));
	
	// <height>.
	::extract(height_, handle.NextSiblingElement("height"));

	// <icon>.
	::extract(icon_, handle.NextSiblingElement("icon"));	

	// <sounds>.
	extract(soundButtonPush_, handle.NextSiblingElement("sounds").FirstChildElement("buttonPush"));
	extract(soundBlockCollision, handle.NextSiblingElement().FirstChildElement("blockCollision"));
	extract(soundRowRemoved, handle.NextSiblingElement().FirstChildElement("rowRemoved"));
	extract(soundTetris, handle.NextSiblingElement().FirstChildElement("tetris"));
	extract(soundHighscore, handle.NextSiblingElement().FirstChildElement("highscore"));

	// <sprites>.
	extract(spriteBackground_, handle.NextSiblingElement("sprites").FirstChildElement("background"));
	extract(spriteComputer_, handle.NextSiblingElement().FirstChildElement("computer"));
	extract(spriteHuman_, handle.NextSiblingElement().FirstChildElement("human"));
	extract(spriteCross_, handle.NextSiblingElement().FirstChildElement("cross"));
	extract(spriteZoom_, handle.NextSiblingElement().FirstChildElement("zoom"));
	extract(spriteZ_, handle.NextSiblingElement().FirstChildElement("squareZ"));
	extract(spriteS_, handle.NextSiblingElement().FirstChildElement("squareS"));
	extract(spriteJ, handle.NextSiblingElement().FirstChildElement("squareJ"));
	extract(spriteI_, handle.NextSiblingElement().FirstChildElement("squareI"));
	extract(spriteL, handle.NextSiblingElement().FirstChildElement("squareL"));
	extract(spriteT, handle.NextSiblingElement().FirstChildElement("squareT"));
	extract(spriteO, handle.NextSiblingElement().FirstChildElement("squareO"));
}

void GameData::loadGame(tinyxml2::XMLHandle handle) {

}

void GameData::loadNetwork(tinyxml2::XMLHandle handle) {

}

void GameData::loadPlayers(tinyxml2::XMLHandle handle) {

}

void GameData::loadHighscore(tinyxml2::XMLHandle handle) {

}

void GameData::extract(mw::Sprite& sprite, tinyxml2::XMLHandle handle) {

}

void GameData::extract(mw::Sound& sound, tinyxml2::XMLHandle handle) {

}

mw::Font GameData::loadFont(std::string file, unsigned int fontSize) {
	unsigned int size = fonts_.size();
	std::string key = file;
	key += fontSize;
	mw::Font& font = fonts_[key];
	
	// Font not found?
	if (fonts_.size() > size) {
		font = mw::Font(file, fontSize);
	}

	return font;
}

mw::Texture GameData::loadTexture(std::string file) {
	unsigned int size = textures_.size();
	mw::Texture& texture = textures_[file];
	
	// Image not found?
	if (textures_.size() > size) {
		texture = mw::Texture(file);

		// Image not valid?
		if (!texture.isValid()) {
			//assert(0);
		}
	}

	return texture;
}

mw::Sound GameData::loadSound(std::string file) {
	unsigned int size = sounds_.size();
	mw::Sound& sound = sounds_[file];
	
	// Sound not found?
	if (sounds_.size() > size) {
		sound = mw::Sound(file);
	}

	return sound;
}
