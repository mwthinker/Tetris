#include "highscore.h"
#include "guiitem.h"
#include "gamefont.h"

#include <mw/text.h>

#include <list>
#include <string>
#include <algorithm>
#include <string>
#include <sstream>

Highscore::Highscore(int nbr, const mw::Color& color) : color_(color) {
	for (int i = 0; i < 10; ++i) {
		mw::Text points("0",fontDefault,25);
		mw::Text name("EMPTY",fontDefault,25);
		mw::Text date("NO DATE",fontDefault,25);
		descList_.push_back(HighscoreElement(0,points,name,date));
	}
	setWidth(50);
	setHeight(nbr * (fontDefault->getCharacterSize()+2));
}

void Highscore::draw() {
	glColor3d(1,1,1);
	glPushMatrix();
	for (auto it = descList_.rbegin(); it != descList_.rend(); ++it) {
		HighscoreElement& highscore = *it;
		mw::Text& points = highscore.points_;
		mw::Text& name = highscore.name_;
		mw::Text& date = highscore.date_;
		glPushMatrix();
		points.draw();
		glTranslated(140,0,0);
		name.draw();
		glTranslated(230,0,0);
		date.draw();
		glPopMatrix();

		glTranslated(0,fontDefault->getCharacterSize()+2,0);
	}
	glPopMatrix();
}

bool Highscore::isNewRecord(int record) const {
	for (const HighscoreElement& highscore : descList_) {
		int points = highscore.intPoints_;
		// New record?
		if (record > points) {
			return true;
		}
	}
	return false;
}

void Highscore::addNewRecord(std::string name, std::string date) {
	mw::Text nameT(name,fontDefault);
	mw::Text dateT(date,fontDefault);
	mw::Text points("0",fontDefault);

	descList_.push_back(HighscoreElement(nextRecord_, points, nameT, dateT));
	sort();
	descList_.pop_back();
}

void Highscore::setNextRecord(int record) {
    nextRecord_ = record;
}

int Highscore::getNextRecord() const {
    return nextRecord_;
}

// Sorts the vector in descending order.
void Highscore::sort() {
	descList_.sort([](const HighscoreElement& a, const HighscoreElement& b) {
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
	return HighscorePtr(new Highscore(nbr,color));
}
