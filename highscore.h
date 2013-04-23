#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include "guiitem.h"
#include "gamefont.h"

#include <mw/text.h>

#include <list>
#include <tuple>
#include <string>
#include <algorithm>

#include <memory>

typedef std::tuple<int,mw::Text,mw::Text,mw::Text> HighscoreElement;

class Highscore : public gui::GuiItem {
public:
	Highscore(int nbr) {
		for (int i = 0; i < 0; ++i) {
			mw::Text points("0",fontDefault);
			mw::Text name("EMPTY",fontDefault);
			mw::Text date("NO DATE",fontDefault);

			descList_.push_back(HighscoreElement(0,points,name,date));
		}

		setWidth(50);
		setHeight(nbr * (fontDefault->getCharacterSize()+2));
	}

	void draw() override {
		glColor3d(1,1,1);
		glPushMatrix();
		for (auto it = descList_.rbegin(); it != descList_.rend(); ++it) {
			HighscoreElement& highscore = *it;
			mw::Text& points = std::get<1>(highscore);
			mw::Text& name = std::get<2>(highscore);
			mw::Text& date = std::get<3>(highscore);
			glPushMatrix();
			points.draw();
			glTranslated(50,0,0);
			name.draw();
			glTranslated(50,0,0);
			date.draw();
			glPopMatrix();

			glTranslated(0,fontDefault->getCharacterSize()+2,0);
		}
		glPopMatrix();
	}

	bool isNewRecord(int record) const {
		for (const HighscoreElement& highscore : descList_) {
			int points = std::get<0>(highscore);
			// New record?
			if (record > points) {
				return true;
			}
		}
		return false;
	}

	void addNewRecord(int record, std::string name, std::string date) {
		mw::Text nameT(name,fontDefault);
		mw::Text dateT(date,fontDefault);
		mw::Text points("0",fontDefault);

		descList_.push_back(HighscoreElement(record, points, nameT, dateT));
		sort();
		descList_.pop_back();
	}

	const std::list<HighscoreElement>& get() {
		return descList_;
	}

	void setList(const std::list<HighscoreElement>& list) {
		descList_ = list;
		sort();
	}

private:
	// Sorts the vector in descending order.
	void sort() {			
		descList_.sort([](const HighscoreElement& a, const HighscoreElement& b) {
			// Points A lesser than points b?
			if (std::get<0>(a) < std::get<0>(b)) {
				return true;
				// Points A equal points b?
			} else if (std::get<0>(a) == std::get<0>(b)) {
				// A has a later date?
				return std::get<2>(a).getText() > std::get<2>(b).getText();
			}
			// Points B higher than points A.
			return false;
		});
	}

	std::list<HighscoreElement> descList_;
};

typedef std::shared_ptr<Highscore> HighscorePtr;

HighscorePtr createHighscore(int nbr) {
	return HighscorePtr(new Highscore(nbr));
}

#endif // HIGHSCORE_H
