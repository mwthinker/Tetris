#include "highscore.h"
#include "gamefont.h"

#include <mw/text.h>

#include <list>
#include <string>
#include <algorithm>
#include <string>
#include <sstream>

Highscore::Highscore(int nbr, const mw::Color& color) : color_(color) {
	for (int i = 0; i < nbr; ++i) {
		mw::Text points("0", fontDefault30, 25);
		mw::Text name("EMPTY", fontDefault30, 25);
		mw::Text date("2013-01-01", fontDefault30, 25);
		ascList_.push_back(HighscoreElement(0, points, name, date));
		std::stringstream stream;
		stream << nbr - i << ":";
		numbers_.push_back(mw::Text(stream.str(), fontDefault30, 25));
	}
	setPreferredSize(300, (float) nbr * (fontDefault30->getCharacterSize() + 2) + 28);
}

void Highscore::draw(float deltaTime) {
	glColor3d(1, 1, 1);
	glPushMatrix();

	int index = 0;
	for (auto it = ascList_.begin(); it != ascList_.end(); ++it) {
		HighscoreElement& highscore = *it;
		mw::Text& points = highscore.points_;
		mw::Text& name = highscore.name_;
		mw::Text& date = highscore.date_;
		glPushMatrix();
		numbers_[index++].draw();
		glTranslated(50, 0, 0);
		points.draw();
		glTranslated(150, 0, 0);
		name.draw();
		glTranslated(170, 0, 0);
		date.draw();
		glPopMatrix();

		glTranslated(0, fontDefault30->getCharacterSize() + 2, 0);
	}

	static mw::Text pointsH("Points", fontDefault30, 28);
	static mw::Text nameH("Name", fontDefault30, 28);
	static mw::Text dataH("Date", fontDefault30, 28);
	glTranslated(50, 0, 0);
	pointsH.draw();
	glTranslated(150, 0, 0);
	nameH.draw();
	glTranslated(170, 0, 0);
	dataH.draw();

	glPopMatrix();
}

bool Highscore::isNewRecord(int record) const {
	for (const HighscoreElement& highscore : ascList_) {
		int points = highscore.intPoints_;
		// New record?
		if (record > points) {
			return true;
		}
	}
	return false;
}

void Highscore::addNewRecord(std::string name, std::string date) {
	mw::Text nameT(name, fontDefault30, 25);
	mw::Text dateT(date, fontDefault30, 25);

	std::stringstream stream;
	stream << nextRecord_;

	mw::Text points(stream.str(), fontDefault30, 25);
	ascList_.push_front(HighscoreElement(nextRecord_, points, nameT, dateT));
	sort();
	ascList_.pop_front();
}

void Highscore::setNextRecord(int record) {
	nextRecord_ = record;
}

int Highscore::getNextRecord() const {
	return nextRecord_;
}

void Highscore::iterateRecords(std::function<void(int, std::string, std::string)> func) const {
	for (const HighscoreElement& highscore : ascList_) {
		func(highscore.intPoints_, highscore.name_.getText(), highscore.date_.getText());
	}
}

// Sorts the vector in ascending order.
void Highscore::sort() {
	ascList_.sort([](const HighscoreElement& a, const HighscoreElement& b) {
		// Points A lesser than points b?
		if (a.intPoints_ < b.intPoints_) {
			return true;
			// Points A equal points b?
		} else if (a.intPoints_ == b.intPoints_) {
			// A has a later date?
			return a.date_.getText() > b.date_.getText();
		}
		// Points B higher than points A.
		return false;
	});
}

HighscorePtr createHighscore(int nbr, const mw::Color& color) {
	return HighscorePtr(new Highscore(nbr, color));
}
