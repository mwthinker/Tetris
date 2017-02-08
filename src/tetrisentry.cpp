#include "tetrisentry.h"

#include <sstream>
#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace xml {

    // Template specialization. Color must be defined as "(0.1 0.2 0.3)" or "(0.1 0.2 0.3 0.4)"
    // red = 0.1, green = 0.2, blue = 0.3, alpha = 0.4
    template <>
    Color extract(tinyxml2::XMLHandle handle) {
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
        Color color(1,1,1,1);
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

	BlockType charToBlockType(char key) {
		switch (key) {
			case 'z':
				// Fall through.
			case 'Z':
				return BlockType::Z;
			case 'w':
				// Fall through.
			case 'W':
				return BlockType::WALL;
			case 't':
				// Fall through.
			case 'T':
				return BlockType::T;
			case 's':
				// Fall through.
			case 'S':
				return BlockType::S;
			case 'o':
				// Fall through.
			case 'O':
				return BlockType::O;
			case 'l':
				// Fall through.
			case 'L':
				return BlockType::L;
			case 'j':
				// Fall through.
			case 'J':
				return BlockType::J;
			case 'I':
				// Fall through.
			case 'i':
				return BlockType::I;
			default:
				return BlockType::EMPTY;
		}
	}

	template <>
	BlockType extract(tinyxml2::XMLHandle handle) {
		const tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw std::runtime_error("Missing element!");
		}
		const char* str = element->GetText();

		if (str == nullptr) {
			throw std::runtime_error("Missing text!");
		}

		std::stringstream stream(str);
		char chr = ' ';
		stream >> chr;
		return charToBlockType(chr);
	}

	template <>
	std::vector<BlockType> extract(tinyxml2::XMLHandle handle) {
		const tinyxml2::XMLElement* element = handle.ToElement();
		if (element == nullptr) {
			throw std::runtime_error("Missing element!");
		}
		const char* str = element->GetText();

		if (str == nullptr) {
			throw std::runtime_error("Missing text!");
		}
		std::string text = str;

		std::vector<BlockType> blockTypes_;
		for (char key : text) {
			blockTypes_.push_back(charToBlockType(key));
		}
		return blockTypes_;
	}

}

std::ostream & operator<<(std::ostream &out, BlockType type) {
	switch (type) {
		case BlockType::Z:
			return out << 'Z';
		case BlockType::WALL:
			return out << 'W';
		case BlockType::T:
			return out << 'T';
		case BlockType::S:
			return out << 'S';
		case BlockType::O:
			return out << 'O';
		case BlockType::L:
			return out << 'L';
		case BlockType::J:
			return out << 'J';
		case BlockType::I:
			return out << 'I';
		case BlockType::EMPTY:
			return out << 'E';
		default:
			return out << 'E';
	}
}

std::ostream & operator<<(std::ostream &out, const std::vector<BlockType>& vector) {
	for (BlockType type : vector) {
		out << type;
	}
	return out;
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

Color TetrisEntry::getColor() const {
	return get<Color>();
}

Ai TetrisEntry::getAi() const {
	Ai ai(getChildEntry("name").getString(), getChildEntry("valueFunction").getString());
	return ai;
}

BlockType TetrisEntry::getBlockType() const {
	return get<BlockType>();
}

std::vector<BlockType> TetrisEntry::getBlockTypes() const {
	return get<std::vector<BlockType>>();
}

void TetrisEntry::bindTextureFromAtlas() const {
	if (data_) {
		data_->textureAtlas_.getTexture().bindTexture();
	}
}

mw::Sprite TetrisEntry::Data::extractSprite(TetrisEntry entry) const {
	return textureAtlas_.add(entry.getString(), 1);
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
