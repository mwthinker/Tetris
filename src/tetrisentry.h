#ifndef TETRISENTY_H
#define TETRISENTY_H

#include "ai.h"

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/color.h>
#include <mw/music.h>

#include <xml/dataentry.h>

#include <map>
#include <string>
#include <sstream>
#include <cassert>
#include <vector>

class TetrisEntry : public xml::DataEntry {
public:
	TetrisEntry(std::string fileName);

	TetrisEntry getEntry(std::string tagNames) const;

	TetrisEntry getChildEntry(std::string tagName) const;

	TetrisEntry getSibling(std::string siblingName) const;

	mw::Font getFont(int size) const;
	mw::Sound getSound() const;
	mw::Music getMusic() const;
	mw::Sprite getSprite() const;
	mw::Texture getTexture() const;
	mw::Color getColor() const;
	Ai getAi() const;

private:
	class Data {
	public:
		mw::Sprite extractSprite(TetrisEntry entry) const;

		mw::Sound extractSound(TetrisEntry entry) const;

		mw::Music extractMusic(TetrisEntry entry) const;

		mw::Font loadFont(std::string file, unsigned int fontSize) const;
		mw::Sound loadSound(std::string file) const;
		mw::Texture loadTexture(std::string file) const;
		mw::Music loadMusic(std::string file) const;

		mutable std::map<std::string, mw::Texture> textures_;
		mutable std::map<std::string, mw::Sound> sounds_;
		mutable std::map<std::string, mw::Font> fonts_;
		mutable std::map<std::string, mw::Music> musics_;
	};

	TetrisEntry(const TetrisEntry& g, xml::DataEntry e);	

	std::shared_ptr<Data> data_;
};

#endif // TETRISENTY_H
