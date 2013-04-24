#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "guiitem.h"

#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <memory>

class Highscore : public gui::GuiItem {
public:
	Highscore(int nbr, const mw::Color& color);

	void draw() override;

	bool isNewRecord(int record) const;

	void addNewRecord(int record, std::string name, std::string date);	

private:
	struct HighscoreElement {
		HighscoreElement(int intPoints, const mw::Text& points, const mw::Text& name, const mw::Text& date) {
			intPoints_ = intPoints;
			points_ = points;
			name_ = name;
			date_ = date;
		}

		int intPoints_;
		mw::Text points_, name_, date_;
	};

	// Sorts the vector in descending order.
	void sort();

	std::list<HighscoreElement> descList_;
	mw::Color color_;
};

typedef std::shared_ptr<Highscore> HighscorePtr;

HighscorePtr createHighscore(int nbr, const mw::Color&);

#endif // HIGHSCORE_H
