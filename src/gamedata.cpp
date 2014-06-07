#include "gamedata.h"

#include <mw/exception.h>

#include <tinyxml2.h>

#include <sstream>
#include <algorithm>
#include <iostream>

namespace {

	std::stringstream& operator>>(std::stringstream& stream, mw::Color& color) {
		bool start = false;
		bool end = false;
		
		std::string str;
		std::string tmp;
		while (stream >> tmp) {
			if (tmp.size() > 0 && !start) {
				if (tmp[0] == '(') {
					start = true;
					tmp[0] = ' ';
				} else {
					// Failed.
					return stream;
				}
			}
			if (tmp.size() > 0 && !end) {
				for (char& c : tmp) {
					if (c == ')') {
						c = ' ';
						end = true;
						break;
					}
				}
			}
			str += tmp;
			if (start && end) {
				break;
			}
		}

		if (!start || !end) {
			// Failed.
			return stream;
		}

		std::replace(str.begin(), str.end(), ',', ' ');
		std::stringstream newStream_(str);
		newStream_ >> color.red_;
		newStream_ >> color.green_;
		newStream_ >> color.blue_;
		float alpha = 1;
		if (newStream_ >> alpha) {
			color.alpha_ = alpha;
		}

		return stream;
	}

	template <class Output>
	Output extract(tinyxml2::XMLConstHandle handle) {
		const tinyxml2::XMLElement* element = handle.ToElement();
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

	template <class Input>
	void insert(const Input& input, tinyxml2::XMLHandle handle) {
		tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw mw::Exception("Missing element!");
		}

		std::stringstream stream;
		stream << input;

		element->SetText(stream.str().c_str());
	}

}

GameData::GameData(std::string dataFile) : dataFile_(dataFile) {
	xmlDoc_.LoadFile(dataFile.c_str());
	if (xmlDoc_.Error()) {
		// Failed!
		xmlDoc_.PrintError();
	}
	
	tinyxml2::XMLHandle handlemXl = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris"));

	// Load all data.
	load(handlemXl);
}

void GameData::save() {
	xmlDoc_.SaveFile(dataFile_.c_str());
}

void GameData::setWindowSize(int width, int height) {
	tinyxml2::XMLHandle handlemXl = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(width, handlemXl.FirstChildElement("width"));
	::insert(height, handlemXl.FirstChildElement("height"));
	save();
}

int GameData::getWindowWidth() const {
	const tinyxml2::XMLConstHandle handlemXl = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handlemXl.FirstChildElement("width"));
}
int GameData::getWindowHeight() const {
	const tinyxml2::XMLConstHandle handlemXl = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handlemXl.FirstChildElement("height"));
}

std::string GameData::getIconPath() const {
	return icon_;
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
	handle = handle.FirstChildElement("bar");
	::extract(barHeight_, handle.FirstChildElement("height"));
	::extract(barColor_, handle.FirstChildElement("color"));
	
	// <label>.
	handle = handle.NextSiblingElement("label");
	::extract(labelTextColor_, handle.FirstChildElement("textColor"));
	::extract(labelBackgroundColor_, handle.FirstChildElement("backgroundColor"));
	
	// <button>.
	handle = handle.NextSiblingElement("button");
	::extract(buttonFocusColor_, handle.FirstChildElement("focusColor"));
	::extract(buttonTextColor_, handle.FirstChildElement("textColor"));
	::extract(buttonHoverColor_, handle.FirstChildElement("hoverColor"));
	::extract(buttonPushColor_, handle.FirstChildElement("pushColor"));
	::extract(buttonBackgroundColor_, handle.FirstChildElement("backgroundColor"));
	::extract(buttonBorderColor_, handle.FirstChildElement("borderColor"));

	// <font>.
	handle = handle.NextSiblingElement("font");
	::extract(font_, handle);

	// <icon>.
	handle = handle.NextSiblingElement("icon");
	::extract(icon_, handle);

	// <sounds>.
	handle = handle.NextSiblingElement("sounds");
	extract(soundButtonPush_, handle.FirstChildElement("buttonPush"));
	extract(soundBlockCollision_, handle.FirstChildElement("blockCollision"));
	extract(soundRowRemoved_, handle.FirstChildElement("rowRemoved"));
	extract(soundTetris_, handle.FirstChildElement("tetris"));
	extract(soundHighscore_, handle.FirstChildElement("highscore"));

	// <sprites>.
	handle = handle.NextSiblingElement("sprites");
	extract(spriteBackground_, handle.FirstChildElement("background"));
	extract(spriteComputer_, handle.FirstChildElement("computer"));
	extract(spriteHuman_, handle.FirstChildElement("human"));
	extract(spriteCross_, handle.FirstChildElement("cross"));
	extract(spriteZoom_, handle.FirstChildElement("zoom"));
	extract(spriteZ_, handle.FirstChildElement("squareZ"));
	extract(spriteS_, handle.FirstChildElement("squareS"));
	extract(spriteJ_, handle.FirstChildElement("squareJ"));
	extract(spriteI_, handle.FirstChildElement("squareI"));
	extract(spriteL_, handle.FirstChildElement("squareL"));
	extract(spriteT_, handle.FirstChildElement("squareT"));
	extract(spriteO_, handle.FirstChildElement("squareO"));
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
	tinyxml2::XMLElement* element = handle.ToElement();
	if (element == nullptr) {
		throw mw::Exception("Missing element!");
	}

	const char* str = element->GetText();

	if (str == nullptr) {
		throw mw::Exception("Missing text!");
	}

	float x = element->FloatAttribute("x");
	float y = element->FloatAttribute("y");
	float h = element->FloatAttribute("h");
	float w = element->FloatAttribute("w");

	mw::Texture texture = loadTexture(str);

	if (h < 1) {
		h = (float) texture.getHeight();
	}

	if (w < 1) {
		w = (float) texture.getWidth();
	}

	sprite = mw::Sprite(texture, x, y, w, h);
}

void GameData::extract(mw::Sound& sound, tinyxml2::XMLHandle handle) {
	tinyxml2::XMLElement* element = handle.ToElement();
	if (element == nullptr) {
		throw mw::Exception("Missing element!");
	}

	const char* str = element->GetText();

	if (str != nullptr) {
		sound = loadSound(str);
	}
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
			std::cerr << file << " failed to load!" << std::endl;
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
