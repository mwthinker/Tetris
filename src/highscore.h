#ifndef HIGHSCORE_H
#define HIGHSCORE_H


#include <gui/component.h>
#include <mw/text.h>
#include <mw/color.h>

#include <list>
#include <string>
#include <memory>
#include <vector>

class Highscore : public gui::Component {
public:
	Highscore(int nbr, const mw::Color& color, const mw::Font& font);

	void draw(Uint32 deltaTime) override;

	bool isNewRecord(int record) const;
	void addNewRecord(std::string name, std::string date);

	void setNextRecord(int record);
	int getNextRecord() const;

	void iterateRecords(std::function<void(int points, std::string name, std::string date)> func) const;

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

	std::vector<mw::Text> numbers_;
	std::list<HighscoreElement> ascList_;
	mw::Color color_;
	int nextRecord_;
	mw::Font font_;
};

typedef std::shared_ptr<Highscore> HighscorePtr;

HighscorePtr createHighscore(int nbr, const mw::Color&);

#endif // HIGHSCORE_H
