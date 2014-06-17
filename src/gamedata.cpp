#include "gamedata.h"

#include <mw/exception.h>

#include <tinyxml2.h>

#include <sstream>
#include <algorithm>
#include <iostream>

namespace {

	// Override the stream operator for a color defined as a string "(r,g,b)".
	// Any space character will be ignored but r, g, b represent a float value 
	// and must be seperated by "," and the whole string surrounded by "(" and ")".
	// Example of correct usaged: 
	// stringstream stream;
	// stream << "( 1.0, 0.1,2);
	// Color color;
	// stream >> color;	
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
					stream.setstate(std::ios::failbit);
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
			stream.setstate(std::ios::failbit);
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
		if (stream.fail()) {
			throw mw::Exception("Stream failed!");
		}
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
		if (stream.fail()) {
			throw mw::Exception("Stream failed!");
		}
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
		if (stream.fail()) {
			throw mw::Exception("Stream failed!");
		}

		element->SetText(stream.str().c_str());
	}

}

GameData::GameData(std::string dataFile) : dataFile_(dataFile) {
	xmlDoc_.LoadFile(dataFile.c_str());
	if (xmlDoc_.Error()) {
		// Failed!
		xmlDoc_.PrintError();
	}
	
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris"));

	// Load all data.
	load(handleXml);
}

void GameData::save() {
	xmlDoc_.SaveFile(dataFile_.c_str());
}

void GameData::setWindowSize(int width, int height) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(width, handleXml.FirstChildElement("width"));
	::insert(height, handleXml.FirstChildElement("height"));
	save();
}

int GameData::getWindowWidth() const {
	tinyxml2::XMLConstHandle handlemXl = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handlemXl.FirstChildElement("width"));
}
int GameData::getWindowHeight() const {
	const tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handleXml.FirstChildElement("height"));
}

void GameData::setWindowPosition(int x, int y) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(x, handleXml.FirstChildElement("positionX"));
	::insert(y, handleXml.FirstChildElement("positionY"));
	save();
}

int GameData::getWindowXPosition() const {
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handleXml.FirstChildElement("positionX"));
}

int GameData::getWindowYPosition() const {
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<int>(handleXml.FirstChildElement("positionY"));
}

void GameData::setWindowMaximized(bool maximized) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(maximized, handleXml.FirstChildElement("maximized"));
	save();
}

bool GameData::isWindowMaximized() const {
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	return ::extract<bool>(handleXml.FirstChildElement("maximized"));
}

std::vector<Ai> GameData::getAis() const {
	std::vector<Ai> v;
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("ais").FirstChildElement("player");
	while (handleXml.ToElement() != nullptr) {
		std::string name = ::extract<std::string>(handleXml.FirstChildElement("name"));
		std::string valueFunction = ::extract<std::string>(handleXml.FirstChildElement("valueFunction"));
		v.push_back(Ai(name, valueFunction));
		handleXml = handleXml.NextSibling();
	}
	return v;
}

std::vector<GameData::Highscore> GameData::getHighscoreVector() const {
	std::vector<GameData::Highscore> v;
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("highscore").FirstChildElement("item");
	while (handleXml.ToElement() != nullptr) {		
		std::string name = ::extract<std::string>(handleXml.FirstChildElement("name"));
		std::string date = ::extract<std::string>(handleXml.FirstChildElement("date"));
		int points = ::extract<int>(handleXml.FirstChildElement("points"));
		v.push_back(Highscore(name, points, date));
		handleXml = handleXml.NextSibling();
	}
	return v;
}

void GameData::setHighscoreVector(const std::vector<GameData::Highscore>& v) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("highscore");
	tinyxml2::XMLNode* node = handleXml.ToNode();
	if (node != nullptr) {
		node->DeleteChildren();
		tinyxml2::XMLNode* element = nullptr;
		for (const GameData::Highscore& highscore : v) {
			if (nullptr == element) {
				// Only first time.
				element = xmlDoc_.NewElement("item");
				node->InsertFirstChild(element);
			} else {
				element = node->InsertAfterChild(element, xmlDoc_.NewElement("item"));
			}			
			
			auto name = xmlDoc_.NewElement("name");
			name->SetText(highscore.name_.c_str());
			
			auto points = xmlDoc_.NewElement("points");
			points->SetText(highscore.points_);
			
			auto date = xmlDoc_.NewElement("date");
			date->SetText(highscore.date_.c_str());

			element->InsertFirstChild(name);
			element->InsertAfterChild(name, points);
			element->InsertAfterChild(points, date);
		}
	}
	save();
}

std::string GameData::getIconPath() const {
	return icon_;
}

void GameData::load(tinyxml2::XMLHandle handle) {
	loadWindow(handle.FirstChildElement("window"));
	loadGame(handle.FirstChildElement("game"));
	loadNetwork(handle.FirstChildElement("network"));
	loadPlayers(handle.FirstChildElement("players"));
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
