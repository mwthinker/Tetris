#include "tetrisentry.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace xml {

    // Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
    // red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
    template <>
    mw::Color extract(tinyxml2::XMLHandle handle) {
        const tinyxml2::XMLElement* element = handle.ToElement();
        if (element == nullptr) {
            throw std::runtime_error("Missing element!");
        }
        const char* str = element->GetText();

        if (str == nullptr) {
            throw std::runtime_error("Missing text!");
        }

        std::stringstream stream(str);
        char chr = 0;
        mw::Color color(1,1,1,1);
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
        float alpha;
        if (stream >> alpha) {
            // Assume that everything is correct.
            color.alpha_ = alpha;
        }

        return color;
    }

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
