#ifndef HIGHSCORE_H
#define HIGHSCORE_H

#include <gui/component.h>

#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <vector>

class Highscore : public gui::Component {
public:
	struct HighscoreElement {
		HighscoreElement(int intPoints, const mw::Text& points,
			const mw::Text& name, const mw::Text& date);

		int intPoints_;
		mw::Text points_, name_, date_;
	};

	Highscore(int nbr, const mw::Color& color, const mw::Font& font);

	void draw(const gui::Graphic& graphic, double deltaTime) override;

	bool isNewRecord(int record) const;
	void addNewRecord(std::string name, std::string date);

	void setNextRecord(int record);
	int getNextRecord() const;

	std::list<HighscoreElement>::const_iterator begin() const {
		return ascList_.cbegin();
	}

	std::list<HighscoreElement>::const_iterator end() const {
		return ascList_.cend();
	}

private:
	// Sorts the vector in descending order.
	static void sortAsc(std::list<HighscoreElement>& ascList);

	mw::Text pointsHeader_, nameHeader_, dateHeader_;

	std::vector<mw::Text> numbers_;
	std::list<HighscoreElement> ascList_;
	mw::Color color_;
	int nextRecord_;
	mw::Font font_;
};

#endif // HIGHSCORE_H
