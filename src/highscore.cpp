#include "highscore.h"

#include <mw/text.h>

#include <list>
#include <string>
#include <algorithm>
#include <string>
#include <sstream>

Highscore::Highscore(int nbr, const mw::Color& color, const mw::Font& font) : color_(color), font_(font) {
	for (int i = 0; i < nbr; ++i) {
		std::stringstream stream;
		stream << nbr - i << ":";
		numbers_.push_back(mw::Text(stream.str(), font_, 25));
	}
	setPreferredSize(300, (float) nbr * (font_.getCharacterSize() + 2) + 28);
}

void Highscore::draw(Uint32 deltaTime) {
	auto wp = getWindowMatrixPtr();
	auto old = wp->getModel();
	wp->setColor(1, 1, 1);

	int index = 0;
	for (auto it = ascList_.begin(); it != ascList_.end(); ++it) {
		HighscoreElement& highscore = *it;
		mw::Text& points = highscore.points_;
		mw::Text& name = highscore.name_;
		mw::Text& date = highscore.date_;		
		
		wp->setModel(old * mw::getTranslateMatrix44(5, 5));
		numbers_[index++].draw();
		wp->setModel(old * mw::getTranslateMatrix44(50, 0));
		points.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix44(150, 0));
		name.draw();
		wp->setModel(wp->getModel() * mw::getTranslateMatrix44(170, 0));
		date.draw();

		wp->setModel(old * mw::getTranslateMatrix44((float) font_.getCharacterSize() + 2, 0));
	}

	static mw::Text pointsH("Points", font_, 28);
	static mw::Text nameH("Name", font_, 28);
	static mw::Text dataH("Date", font_, 28);
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(50, 0));
	pointsH.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(150, 0));
	nameH.draw();
	wp->setModel(wp->getModel() * mw::getTranslateMatrix44(150, 0));
	dataH.draw();
	wp->setModel(old);
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
	mw::Text nameT(name, font_, 25);
	mw::Text dateT(date, font_, 25);

	std::stringstream stream;
	stream << nextRecord_;

	mw::Text points(stream.str(), font_, 25);
	ascList_.push_front(HighscoreElement(nextRecord_, points, nameT, dateT));
	sort();
	if (numbers_.size() < ascList_.size()) {
		ascList_.pop_front();
	}
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
