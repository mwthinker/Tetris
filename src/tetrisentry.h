#ifndef TETRISENTY_H
#define TETRISENTY_H

#include "ai.h"

#include <mw/sound.h>
#include <mw/sprite.h>
#include <mw/font.h>
#include <mw/color.h>
#include <mw/music.h>
#include <mw/textureatlas.h>

#include <xml/dataentry.h>

#include <map>

class TetrisEntry : public xml::DataEntry {
public:
	TetrisEntry(std::string fileName);

	TetrisEntry getDeepChildEntry(std::string tagNames) const;

	TetrisEntry getChildEntry(std::string tagName) const;

	TetrisEntry getSibling(std::string siblingName) const;

	mw::Font getFont(int size) const;
	mw::Sound getSound() const;
	mw::Music getMusic() const;
	mw::Sprite getSprite() const;
	mw::Color getColor() const;
	Ai getAi() const;

	void bindTextureFromAtlas() const;

private:
	class Data {
	public:
		inline Data() : textureAtlas_(2048, 2048, []() {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}) {
		}

		mw::Sprite extractSprite(TetrisEntry entry) const;

		mw::Sound extractSound(TetrisEntry entry) const;

		mw::Music extractMusic(TetrisEntry entry) const;

		mw::Font loadFont(std::string file, unsigned int fontSize) const;
		mw::Sound loadSound(std::string file) const;
		mw::Music loadMusic(std::string file) const;

		mutable mw::TextureAtlas textureAtlas_;
		mutable std::map<std::string, mw::Sound> sounds_;
		mutable std::map<std::string, mw::Font> fonts_;
		mutable std::map<std::string, mw::Music> musics_;
	};

	TetrisEntry(const TetrisEntry& g, xml::DataEntry e);

	std::shared_ptr<Data> data_;
};

#endif // TETRISENTY_H
