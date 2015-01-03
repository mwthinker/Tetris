#include "tetrisentry.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>

// Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
// red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
template <>
mw::Color xml::extract(tinyxml2::XMLHandle handle) {
	const tinyxml2::XMLElement* element = handle.ToElement();
	if (element == nullptr) {
		throw std::runtime_error("Missing element!");
	}
	const char* str = element->GetText();

	if (str == nullptr) {
		throw std::runtime_error("Missing text!");
	}

	std::stringstream stream(str);
	char chr = 0;;
	mw::Color color;
	stream >> chr;
	if (chr != '(') {
		throw std::runtime_error("Missing '('!");
	}
	if (!(stream >> color.red_)) {
		throw std::runtime_error("Red value invalid");
	}
	if (!(stream >> color.green_)) {
		throw std::runtime_error("Green value invalid");
	}
	if (!(stream >> color.blue_)) {
		throw std::runtime_error("Blue value invalid");
	}
	// Assume that everything is correct.
	stream >> color.alpha_;
	return color;
}

TetrisEntry::TetrisEntry(std::string fileName) : xml::DataEntry(fileName), data_(std::make_shared<Data>()) {
}

TetrisEntry TetrisEntry::getDeepChildEntry(std::string tagNames) const {
	return TetrisEntry(*this, xml::DataEntry::getDeepChildEntry(tagNames));
}

TetrisEntry TetrisEntry::getChildEntry(std::string tagName) const {
	return TetrisEntry(*this, xml::DataEntry::getChildEntry(tagName));
}

TetrisEntry TetrisEntry::getSibling(std::string siblingName) const {
	return TetrisEntry(*this, DataEntry::getSibling(siblingName));
}

TetrisEntry::TetrisEntry(const TetrisEntry& g, xml::DataEntry e) : xml::DataEntry(e) {
	data_ = g.data_;
}

mw::Font TetrisEntry::getFont(int size) const {
	return data_->loadFont(getString(), size);
}

mw::Sound TetrisEntry::getSound() const {
	return data_->extractSound(*this);
}

mw::Music TetrisEntry::getMusic() const {
	return data_->extractMusic(*this);
}

mw::Sprite TetrisEntry::getSprite() const {
	return data_->extractSprite(*this);
}

mw::Color TetrisEntry::getColor() const {
	return get<mw::Color>();
}

Ai TetrisEntry::getAi() const {
	Ai ai(getChildEntry("name").getString(), getChildEntry("valueFunction").getString());
	return ai;
}

mw::Sprite TetrisEntry::Data::extractSprite(TetrisEntry entry) const {
	return textureAtlas_.add(entry.getString(), 4);
}

mw::Sound TetrisEntry::Data::extractSound(TetrisEntry entry) const {
	return loadSound(entry.getString());
}

mw::Music TetrisEntry::Data::extractMusic(TetrisEntry entry) const {
	return loadMusic(entry.getString());
}

mw::Font TetrisEntry::Data::loadFont(std::string file, unsigned int fontSize) const {
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

mw::Sound TetrisEntry::Data::loadSound(std::string file) const {
	unsigned int size = sounds_.size();
	mw::Sound& sound = sounds_[file];

	// Sound not found?
	if (sounds_.size() > size) {
		sound = mw::Sound(file);
	}

	return sound;
}

mw::Music TetrisEntry::Data::loadMusic(std::string file) const {
	unsigned int size = musics_.size();
	mw::Music& music = musics_[file];

	// Music not found?
	if (musics_.size() > size) {
		music = mw::Music(file);
	}

	return music;
}
/*
GameData::GameData(std::string dataFile) : dataFile_(dataFile) {
	xmlDoc_.LoadFile(dataFile.c_str());
	if (xmlDoc_.Error()) {
		// Failed!
		xmlDoc_.PrintError();
	}
	
	tinyxml2::XMLConstHandle handleXml = tinyxml2::XMLConstHandle(xmlDoc_.FirstChildElement("tetris"));

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

bool GameData::getWindowBorder() const {
	return ::getValueFromTag<bool>(xmlDoc_, "tetris window border");
}

int GameData::getWindowWidth() const {
	return ::getValueFromTag<int>(xmlDoc_, "tetris window width");
}

int GameData::getWindowHeight() const {
	return ::getValueFromTag<int>(xmlDoc_, "tetris window height");
}

void GameData::setWindowPosition(int x, int y) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(x, handleXml.FirstChildElement("positionX"));
	::insert(y, handleXml.FirstChildElement("positionY"));
	save();
}

int GameData::getWindowXPosition() const {
	return ::getValueFromTag<int>(xmlDoc_, "tetris window positionX");
}

int GameData::getWindowYPosition() const {
	return ::getValueFromTag<int>(xmlDoc_, "tetris window positionY");
}

void GameData::setWindowMaximized(bool maximized) {
	tinyxml2::XMLHandle handleXml = tinyxml2::XMLHandle(xmlDoc_.FirstChildElement("tetris")).FirstChildElement("window");
	::insert(maximized, handleXml.FirstChildElement("maximized"));
	save();
}

bool GameData::isWindowMaximized() const {
	return ::getValueFromTag<bool>(xmlDoc_, "tetris window maximized");
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

void GameData::load(tinyxml2::XMLConstHandle handle) {
	loadWindow(handle.FirstChildElement("window"));
	loadGame(handle.FirstChildElement("game"));
	loadNetwork(handle.FirstChildElement("network"));
	loadPlayers(handle.FirstChildElement("players"));
}

void GameData::loadWindow(tinyxml2::XMLConstHandle handle) {
	// <bar>.
	handle = handle.FirstChildElement("bar");
	barHeight_ = ::extract<float>(handle.FirstChildElement("height"));
	barColor_ = ::extract<mw::Color>(handle.FirstChildElement("color"));
	
	// <label>.
	handle = handle.NextSiblingElement("label");
	labelTextColor_ = ::extract<mw::Color>(handle.FirstChildElement("textColor"));
	labelBackgroundColor_ = ::extract<mw::Color>(handle.FirstChildElement("backgroundColor"));
	
	// <button>.
	handle = handle.NextSiblingElement("button");
	buttonFocusColor_ = ::extract<mw::Color>(handle.FirstChildElement("focusColor"));
	buttonTextColor_ = ::extract<mw::Color>(handle.FirstChildElement("textColor"));
	buttonHoverColor_ = ::extract<mw::Color>(handle.FirstChildElement("hoverColor"));
	buttonPushColor_ = ::extract<mw::Color>(handle.FirstChildElement("pushColor"));
	buttonBackgroundColor_ = ::extract<mw::Color>(handle.FirstChildElement("backgroundColor"));
	buttonBorderColor_ = ::extract<mw::Color>(handle.FirstChildElement("borderColor"));

	// <font>.
	handle = handle.NextSiblingElement("font");
	font_ = ::extract<std::string>(handle);

	// <icon>.
	handle = handle.NextSiblingElement("icon");
	icon_ = ::extract<std::string>(handle);

	// <sounds>.
	handle = handle.NextSiblingElement("sounds");
	soundButtonPush_ = extractSound(handle.FirstChildElement("buttonPush"));
	soundBlockCollision_ = extractSound(handle.FirstChildElement("blockCollision"));
	soundRowRemoved_ = extractSound(handle.FirstChildElement("rowRemoved"));
	soundTetris_ = extractSound(handle.FirstChildElement("tetris"));
	soundHighscore_ = extractSound(handle.FirstChildElement("highscore"));

	// <sprites>.
	handle = handle.NextSiblingElement("sprites");
	spriteBackground_ = extractSprite(handle.FirstChildElement("background"));
	spriteComputer_ = extractSprite(handle.FirstChildElement("computer"));
	spriteHuman_ = extractSprite(handle.FirstChildElement("human"));
	spriteCross_ = extractSprite(handle.FirstChildElement("cross"));
	spriteZoom_ = extractSprite(handle.FirstChildElement("zoom"));
	spriteZ_ = extractSprite(handle.FirstChildElement("squareZ"));
	spriteS_ = extractSprite(handle.FirstChildElement("squareS"));
	spriteJ_ = extractSprite(handle.FirstChildElement("squareJ"));
	spriteI_ = extractSprite(handle.FirstChildElement("squareI"));
	spriteL_ = extractSprite(handle.FirstChildElement("squareL"));
	spriteT_ = extractSprite(handle.FirstChildElement("squareT"));
	spriteO_ = extractSprite(handle.FirstChildElement("squareO"));
}

void GameData::loadGame(tinyxml2::XMLConstHandle handle) const {

}

void GameData::loadNetwork(tinyxml2::XMLConstHandle handle) const {

}

void GameData::loadPlayers(tinyxml2::XMLConstHandle handle) const {

}

mw::Sprite GameData::extractSprite(tinyxml2::XMLConstHandle handle) const {
	const tinyxml2::XMLElement* element = handle.ToElement();
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

	return mw::Sprite(texture, x, y, w, h);
}

mw::Sound GameData::extractSound(tinyxml2::XMLConstHandle handle) const {
	const tinyxml2::XMLElement* element = handle.ToElement();
	if (element == nullptr) {
		throw mw::Exception("Missing element!");
	}

	const char* str = element->GetText();

	if (str != nullptr) {
		return loadSound(str);
	}
	return mw::Sound();
}

mw::Font GameData::loadFont(std::string file, unsigned int fontSize)  const {
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

mw::Texture GameData::loadTexture(std::string file) const {
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

mw::Sound GameData::loadSound(std::string file) const {
	unsigned int size = sounds_.size();
	mw::Sound& sound = sounds_[file];
	
	// Sound not found?
	if (sounds_.size() > size) {
		sound = mw::Sound(file);
	}

	return sound;
}
*/