#include "highscore.h"

#include <algorithm>
#include <sstream>

Highscore::HighscoreElement::HighscoreElement(int points, int rows, int level,
	std::string name, std::string date, const mw::Font& font) :

	intPoints_(points),
	intLevel_(level),
	intRows_(rows),

	points_(std::to_string(points), font),
	level_(std::to_string(level), font),
	rows_(std::to_string(rows), font),

	name_(name, font),
	date_(date, font) {
}

Highscore::Highscore(int nbr, const mw::Color& color, const mw::Font& font) : color_(color), font_(font) {
	for (int i = 1; i <= nbr; ++i) {
		int ranking = nbr - i + 1;
		if (ranking < 10) {
			numbers_.push_back(mw::Text(std::string("  ") + std::to_string(ranking) + ": ", font_));
		} else {
			numbers_.push_back(mw::Text(std::to_string(ranking) + ": ", font_));
		}
	}
	setPreferredSize(300, (float) nbr * (font_.getCharacterSize() + 2));

	ranking_ = mw::Text("Ranking", font_);
	pointsHeader_ = mw::Text("Points", font_);
	nameHeader_ = mw::Text("Name", font_);
	dateHeader_ = mw::Text("Date", font_);
	levelheader_ = mw::Text("Level", font_);
	rowsHeader_ = mw::Text("Rows", font_);
}

void Highscore::draw(const gui::Graphic& graphic, double deltaTime) {
	graphic.setColor(1, 1, 1);

	gui::Dimension size = getSize();

	float xArray[] = {5, 100, 220, 355, 415, 480};
	float y = size.height_ - (float) (ascList_.size() + 1) * (5 + font_.getCharacterSize() + 2);
	int index = 0;
	for (auto& highscore : ascList_) {
		y += 5;
		graphic.drawText(numbers_[index++], xArray[0], y - 10);
		graphic.drawText(highscore.points_, xArray[1], y - 10);
		graphic.drawText(highscore.name_, xArray[2], y - 10);
		graphic.drawText(highscore.rows_, xArray[3], y - 10);
		graphic.drawText(highscore.level_, xArray[4], y - 10);
		graphic.drawText(highscore.date_, xArray[5], y - 10);
		y += font_.getCharacterSize() + 2;
	}
	graphic.drawText(ranking_, xArray[0], y);
	graphic.drawText(pointsHeader_, xArray[1], y);
	graphic.drawText(nameHeader_, xArray[2], y);
	graphic.drawText(rowsHeader_, xArray[3], y);
	graphic.drawText(levelheader_, xArray[4], y);
	graphic.drawText(dateHeader_, xArray[5], y);
}

int Highscore::getNextPosition() const {
	int position = ascList_.size() + 1;
	for (const HighscoreElement& highscore : ascList_) {
		int points = highscore.intPoints_;
		// New record?
		if (nextRecord_ > points) {
			--position;
		}
	}
	return position;
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
	ascList_.push_front(HighscoreElement(nextRecord_, nextRows_, nextLevel_, name, date, font_));
	// Same points, older will be ranked higher.
	sortAsc(ascList_);

	if (numbers_.size() < ascList_.size()) {
		ascList_.pop_front();
	}
}

void Highscore::setNextRecord(int record, int rows, int level) {
	nextRecord_ = record;
	nextLevel_ = level;
	nextRows_ = rows;
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
