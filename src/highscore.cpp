#include "highscore.h"

#include <algorithm>
#include <sstream>

Highscore::HighscoreElement::HighscoreElement(int intPoints, const mw::Text& points, const mw::Text& name, const mw::Text& date) :
	intPoints_(intPoints),
	points_(points),
	name_(name),
	date_(date) {

}

Highscore::Highscore(int nbr, const mw::Color& color, const mw::Font& font) : color_(color), font_(font) {
	for (int i = 0; i < nbr; ++i) {
		std::stringstream stream;
		stream << nbr - i << ":";
		numbers_.push_back(mw::Text(stream.str(), font_));
	}
	setPreferredSize(300, (float) nbr * (font_.getCharacterSize() + 2));

	pointsHeader_ = mw::Text("Points", font_);
	nameHeader_ = mw::Text("Name", font_);
	dateHeader_ = mw::Text("Date", font_);
}

void Highscore::draw(const gui::Graphic& graphic, double deltaTime) {
	graphic.setColor(1, 1, 1);

	gui::Dimension size = getSize();

	int index = 0;
	float x = 0;
	float y = size.height_ - (float) (ascList_.size() + 1) * (5 + font_.getCharacterSize() + 2);
	for (auto& highscore : ascList_) {
		mw::Text& points = highscore.points_;
		mw::Text& name = highscore.name_;
		mw::Text& date = highscore.date_;

		x = 5;
		y += 5;
		graphic.drawText(numbers_[index++], x, y);

		x += 50;
		graphic.drawText(points, x, y);
		x += 150;
		graphic.drawText(name, x, y);
		x += 170;
		graphic.drawText(date, x, y);
		y += font_.getCharacterSize() + 2;
	}

	x = 5;
	graphic.drawText(pointsHeader_, x + 50, y);
	graphic.drawText(nameHeader_, x + 50 + 150, y);
	graphic.drawText(dateHeader_, x + 50 + 150 + 170, y);
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
	mw::Text nameT(name, font_);
	mw::Text dateT(date, font_);

	std::stringstream stream;
	stream << nextRecord_;

	mw::Text pointsT(stream.str(), font_);
	ascList_.push_back(HighscoreElement(nextRecord_, pointsT, nameT, dateT));
	sortAsc(ascList_);

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

// Sorts the vector in ascending order.
void Highscore::sortAsc(std::list<HighscoreElement>& ascList) {
	ascList.sort([](const HighscoreElement& a, const HighscoreElement& b) {
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
